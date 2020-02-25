//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#include "renderer.h"
#include "../voxelOctree.h"

#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/backend/pipeline.h>
#include <graphics/backend/renderPass.h>
#include <graphics/backend/texture2d.h>
#include <graphics/scene/camera.h>
#include <graphics/Image.h>
#include <graphics/debug/imgui.h>
#include <graphics/debug/debugGUI.h>

#include <string>
#include <sstream>

using namespace rev::gfx;
using namespace rev::math;

namespace vkft::gfx
{
	//------------------------------------------------------------------------------------------------------------------
	Renderer::Renderer(rev::gfx::DeviceOpenGLWindows& device, const Vec2u& targetSize)
		: mGfxDevice(device)
		, m_noisePermutations(0,NumBlueNoiseTextures-1)
		, m_rasterPass(device)
	{
		loadNoiseTextures();
		loadTexturePack();
		// Create reusable texture descriptor
		TextureSampler::Descriptor bufferSamplerDesc;
		bufferSamplerDesc.filter = TextureSampler::MinFilter::Nearest;
		bufferSamplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		bufferSamplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		m_rtBufferSampler = mGfxDevice.createTextureSampler(bufferSamplerDesc);

		// Create an intermediate buffer of the right size
		onResizeTarget(targetSize);

		// Create the full screen pass to draw the result
		m_rasterCode = ShaderCodeFragment::loadFromFile("raster.fx");
		m_reloadListeners.push_back(m_rasterCode->onReload([this](ShaderCodeFragment& fragment){
				m_rasterPass.setPassCode(&fragment);
		}));
		m_rasterPass.setPassCode(m_rasterCode);

		RenderPass::Descriptor fullScreenDesc;
		float grey = 0.5f;
		fullScreenDesc.clearColor = { grey,grey,grey, 1.f };
		fullScreenDesc.clearDepth = 1.0;
		fullScreenDesc.clearFlags = RenderPass::Descriptor::Clear::All;
		fullScreenDesc.target = mGfxDevice.defaultFrameBuffer();
		fullScreenDesc.viewportSize = targetSize;
		m_finalPass = mGfxDevice.createRenderPass(fullScreenDesc);

		// Show compute device limits on console
		auto& limits = mGfxDevice.getDeviceLimits();
		std::cout << "Compute group max count: " << limits.computeWorkGroupCount.x() << " " << limits.computeWorkGroupCount.y() << " " << limits.computeWorkGroupCount.z() << "\n";
		std::cout << "Compute group max size: " << limits.computeWorkGroupSize.x() << " " << limits.computeWorkGroupSize.y() << " " << limits.computeWorkGroupSize.z() << "\n";
		std::cout << "Compute max invokations: " << limits.computeWorkGruopTotalInvokes << "\n";

		// Create compute shader
        loadShaderAndSetListener("rtGBuffer.fx", m_gBufferCompute);
        loadShaderAndSetListener("rtDirect.fx", m_directLightCompute);
		loadShaderAndSetListener("rtCompose.fx", m_composeCompute);
		loadShaderAndSetListener("mixHor.fx", m_mixHorCompute);
		loadShaderAndSetListener("mixVer.fx", m_mixVerCompute);
		loadShaderAndSetListener("taa.fx", m_taaCompute);

		m_taaView = Mat44f::identity();

		gui::init(targetSize);
	}

	void Renderer::BoxFilter(
		Texture2d texture,
		const Vec4f& textureSize,
		unsigned iterations,
		CommandBuffer& commands)
	{
		CommandBuffer::UniformBucket passUniforms;
		for (int i = 0; i < iterations; ++i)
		{
			float step = float(2 * i + 1);
			passUniforms.clear();
			passUniforms.addParam(1, textureSize);
			passUniforms.addParam(2, Vec4f(step, 0.f, 0.f, 0.f));
			passUniforms.addParam(10, texture);
			passUniforms.addParam(11, m_gBufferTexture);
			commands.setComputeProgram(m_mixHorCompute);
			commands.setUniformData(passUniforms);
			commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
			commands.dispatchCompute(m_pingPongTexture, Vec3i{ (int)m_targetSize.x()/32+1, (int)m_targetSize.y(), 1 });

			passUniforms.clear();
			passUniforms.addParam(1, textureSize);
			passUniforms.addParam(2, step);
			passUniforms.addParam(10, m_pingPongTexture);
			passUniforms.addParam(11, m_gBufferTexture);
			commands.setComputeProgram(m_mixVerCompute);
			commands.setUniformData(passUniforms);
			commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
			commands.dispatchCompute(texture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y() /32+1, 1 });
		}
	}

	void Renderer::TaaFilter(
		const Vec4f& textureSize,
		float taaConfidence,
		Texture2d gBuffer,
		Texture2d pastFrame,
		Texture2d thisFrame,
		Texture2d target,
		CommandBuffer& commands
		)
	{
		CommandBuffer::UniformBucket passUniforms;

		passUniforms.addParam(1, textureSize);
		passUniforms.addParam(4, taaConfidence);
		passUniforms.addParam(10, gBuffer);
		passUniforms.addParam(11, pastFrame);
		passUniforms.addParam(12, thisFrame);
		commands.setComputeProgram(m_taaCompute);
		commands.setUniformData(passUniforms);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		commands.dispatchCompute(target, Vec3i{ (int)m_targetSize.x() / 4 + 1, (int)m_targetSize.y() / 4 + 1, 1 });
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::render(const vkft::VoxelOctree& worldMap, const rev::gfx::Camera& camera, float dt)
	{
		CommandBuffer commands;
        CommandBuffer::UniformBucket passUniforms;

		if(!m_composeCompute.isValid()
		|| !m_gBufferCompute.isValid()
		|| !m_directLightCompute.isValid())
			return;

		gui::startFrame(m_targetSize);
		ImGui::Begin("Render options");
        // Optimization ideas:
        // - Run a broad phase conservative g-buffer to reduce distance traversed by primary rays
        // - Or just rasterize the g-buffer. Which also allows you to take advantage of normal mapping and all things
        //   that can be done on deferred.
        // - Indirect lighting would use raytraced geometry and discard things like normal mapping. Also simplified
        //   shading, and probably increased roughness. Could even use environment probes for terciary lighting
		// - An alternative approach could be switching between ray tracing and cone tracing for secondary light depending
		//   on "width" of the secondary brdf lobe.
        // - For supporting many lights, do as clustered forward and precompute a list of affecting lights per cluster
        // - Can that list work for both direct and indirect lighting?
		
		// Prepare data
		Vec4f uWindow;
		uWindow.x() = (float)m_targetSize.x();
		uWindow.y() = (float)m_targetSize.y();
        CommandBuffer::UniformBucket commonUniforms;
        commonUniforms.addParam(1, uWindow);
		float aspectRatio = uWindow.x() / uWindow.y();
		Mat44f camWorldMtx = camera.world().matrix();
		Mat44f view = camera.view();
		Mat44f proj = camera.projection(aspectRatio);

        commonUniforms.addParam(2, camWorldMtx);
		commonUniforms.addParam(3, view);
		commonUniforms.addParam(4, proj);
		commonUniforms.addParam(5, m_texturePack);
		commonUniforms.addParam(6, Vec4f(m_noisePermutations(m_rng),m_noisePermutations(m_rng),0.f,0.f));
		commonUniforms.addParam(7, worldMap.gpuBuffer());

		// Dispatch gBuffer shader
		auto raytracingDispatchGroupSize = Vec3i{ (int)m_targetSize.x() / 8 + 1, (int)m_targetSize.y() / 4 + 1, 1 };
		commands.setComputeProgram(m_gBufferCompute);
        commands.setUniformData(commonUniforms);
		commands.dispatchCompute(m_gBufferTexture, raytracingDispatchGroupSize);

		// Dispatch light contributions
		auto lastFrameNdx = m_taaIndex;
		auto thisFrameNdx = m_taaIndex^1;
        passUniforms.clear();
        passUniforms.addParam(11, m_gBufferTexture); // So we can read from the g-buffer
		passUniforms.addComputeOutput(1, m_indirectLightTexture);
        unsigned noiseTextureNdx = m_noisePermutations(m_rng); // New noise permutation for primary light
        passUniforms.addParam(12, m_blueNoise[noiseTextureNdx]);
        commands.setComputeProgram(m_directLightCompute);
        commands.setUniformData(commonUniforms);
        commands.setUniformData(passUniforms);
        commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess); // Wait for G-Buffer to be ready
        commands.dispatchCompute(m_directLightTexture, raytracingDispatchGroupSize);

		// Denoise direct light
		ImGui::SliderInt("Direct Box Filter iterations", &m_directBoxIterations, 0, 10);
		BoxFilter(m_directLightTexture, uWindow, m_directBoxIterations, commands);
		TaaFilter(uWindow, m_taaConfidence, m_gBufferTexture,
			m_directLightTAABuffer[lastFrameNdx], m_directLightTexture,
			m_directLightTAABuffer[thisFrameNdx], commands);

		// Denoise indirect light
		ImGui::SliderInt("Indirect Box Filter iterations", &m_indirectBoxIterations, 0, 10);
		BoxFilter(m_indirectLightTexture, uWindow, m_indirectBoxIterations, commands);
		TaaFilter(uWindow, m_taaConfidence, m_gBufferTexture,
			m_indirectLightTAABuffer[lastFrameNdx], m_indirectLightTexture,
			m_indirectLightTAABuffer[thisFrameNdx], commands);

		// Compose image
		passUniforms.clear();
        passUniforms.addParam(11, m_gBufferTexture);
        passUniforms.addParam(12, m_directLightTAABuffer[thisFrameNdx]);
		if(m_taaConfidence > 0)
			passUniforms.addParam(13, m_indirectLightTAABuffer[thisFrameNdx]);
		else
			passUniforms.addParam(13, m_indirectLightTexture);
		passUniforms.addParam(16, m_taaView);
		m_taaView = view;
		commands.setComputeProgram(m_composeCompute);
        commands.setUniformData(commonUniforms);
        commands.setUniformData(passUniforms);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		commands.dispatchCompute(m_raytracingTexture, raytracingDispatchGroupSize);

		// Render full screen texture
        passUniforms.clear();
        passUniforms.addParam(0, uWindow);
        passUniforms.addParam(1, m_raytracingTexture);
		commands.beginPass(*m_finalPass);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		m_rasterPass.render(passUniforms, commands);

		// Submit
		mGfxDevice.renderQueue().submitCommandBuffer(commands);

		ImGui::End();
		gui::finishFrame(dt);
		ImGui::Render();
		mGfxDevice.renderQueue().present();

		m_taaIndex ^= 1;
		m_taaConfidence = 1.f;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::onResizeTarget(const rev::math::Vec2u& newSize)
	{
		m_taaConfidence = 0.f;
		m_targetSize = newSize;
		mTargetFov = float(newSize.x()) / newSize.y();
		if(m_finalPass)
			m_finalPass->setViewport({0,0}, newSize);

		// Delete previous target if needed
		if(m_raytracingTexture.isValid())
		{
            mGfxDevice.destroyTexture2d(m_gBufferTexture);
            mGfxDevice.destroyTexture2d(m_directLightTexture);
            mGfxDevice.destroyTexture2d(m_indirectLightTexture);
            mGfxDevice.destroyTexture2d(m_raytracingTexture);
		}

		// Create a common descriptor for all buffers
		Texture2d::Descriptor bufferDesc;
		bufferDesc.depth = false;
		bufferDesc.mipLevels = 1;
		bufferDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
		bufferDesc.pixelFormat.numChannels = 4;
		bufferDesc.providedImages = 0;
		bufferDesc.sampler = m_rtBufferSampler;
		bufferDesc.size = newSize;
		bufferDesc.sRGB = false;

        // Create the buffers
        m_gBufferTexture = mGfxDevice.createTexture2d(bufferDesc);
        m_directLightTexture = mGfxDevice.createTexture2d(bufferDesc);
		m_directLightTAABuffer[0] = mGfxDevice.createTexture2d(bufferDesc);
		m_directLightTAABuffer[1] = mGfxDevice.createTexture2d(bufferDesc);
		m_indirectLightTAABuffer[0] = mGfxDevice.createTexture2d(bufferDesc);
		m_indirectLightTAABuffer[1] = mGfxDevice.createTexture2d(bufferDesc);
        m_indirectLightTexture = mGfxDevice.createTexture2d(bufferDesc);
		m_raytracingTexture = mGfxDevice.createTexture2d(bufferDesc);
		m_pingPongTexture = mGfxDevice.createTexture2d(bufferDesc);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::loadTexturePack()
	{
		// Noise texture sampler
		TextureSampler::Descriptor noiseSamplerDesc;
		noiseSamplerDesc.filter = TextureSampler::MinFilter::Linear;
		noiseSamplerDesc.wrapS = TextureSampler::Wrap::Repeat;
		noiseSamplerDesc.wrapT = TextureSampler::Wrap::Repeat;

		rev::gfx::Texture2d::Descriptor texDescriptor;
		texDescriptor.sampler = mGfxDevice.createTextureSampler(noiseSamplerDesc);
		texDescriptor.depth = false;
		texDescriptor.mipLevels = 0;
		texDescriptor.pixelFormat.channel = Image::ChannelFormat::Byte;
		texDescriptor.pixelFormat.numChannels = 4;
		texDescriptor.providedImages = 1;
		texDescriptor.size = Vec2u(256,256);
		texDescriptor.sRGB = true;

		auto image = Image::load("minecraft.jpg", 0);
		if(image.data())
		{
			texDescriptor.srcImages = &image;
			m_texturePack = mGfxDevice.createTexture2d(texDescriptor);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::loadNoiseTextures()
	{
		// Noise texture sampler
		TextureSampler::Descriptor noiseSamplerDesc;
		noiseSamplerDesc.filter = TextureSampler::MinFilter::Nearest;
		noiseSamplerDesc.wrapS = TextureSampler::Wrap::Repeat;
		noiseSamplerDesc.wrapT = TextureSampler::Wrap::Repeat;

		rev::gfx::Texture2d::Descriptor m_noiseDesc;
		m_noiseDesc.sampler = mGfxDevice.createTextureSampler(noiseSamplerDesc);
		m_noiseDesc.depth = false;
		m_noiseDesc.mipLevels = 1;
		m_noiseDesc.pixelFormat.channel = Image::ChannelFormat::Byte;
		m_noiseDesc.pixelFormat.numChannels = 4;
		m_noiseDesc.providedImages = 1;
		m_noiseDesc.size = Vec2u(64,64);
		m_noiseDesc.sRGB = false;

		for(unsigned i = 0; i < NumBlueNoiseTextures; ++i)
		{
#if 1
			// Create a brand new noise texture
			Vec4f* noise = new Vec4f[64*64];
			std::uniform_real_distribution<float> noiseDistrib;
			for(int i = 0; i < 64*64; ++i)
			{
				noise[i].x() = noiseDistrib(m_rng);
				noise[i].y() = noiseDistrib(m_rng);
				noise[i].z() = noiseDistrib(m_rng);
				noise[i].w() = noiseDistrib(m_rng);
			}
			auto image = new rev::gfx::Image(m_noiseDesc.size, noise);
			m_noiseDesc.srcImages = image;
			m_blueNoise[i] = mGfxDevice.createTexture2d(m_noiseDesc);
			delete image;
#else
			// Load image from file
			std::stringstream imageName;
			imageName << "../assets/blueNoise/LDR_RGBA_" << i << ".png";
			auto image = Image::load(imageName.str(), 0);
			if(image.data())
			{
				m_noiseDesc.srcImages = &image;
				m_blueNoise[i] = mGfxDevice.createTexture2d(m_noiseDesc);
			}
#endif
		}
	}

    //------------------------------------------------------------------------------------------------------------------
    void Renderer::loadShaderAndSetListener(const std::string& shaderFile, ComputeShader& dst)
    {
        m_computeCode.push_back(nullptr);
        auto& dstFragment = m_computeCode.back();
        dstFragment = ShaderCodeFragment::loadFromFile(shaderFile);
        std::vector<std::string> code;
        dstFragment->collapse(code);
        m_reloadListeners.emplace_back(dstFragment->onReload([this,&dst](ShaderCodeFragment& fragment) {
            std::vector<std::string> code;
            fragment.collapse(code);
            auto newCompute = mGfxDevice.createComputeShader(code);
            if (newCompute.isValid())
                dst = newCompute;
        }));

        dst = mGfxDevice.createComputeShader(code);
    }
}