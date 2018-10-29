//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "ForwardPass.h"
#include "ShadowMapPass.h"

#include <core/platform/fileSystem/file.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/time/time.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/renderPass.h>
#include <graphics/driver/shaderProcessor.h>
#include <graphics/renderer/material/material.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/EnvironmentProbe.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderMesh.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderScene.h>
#include <math/algebra/affineTransform.h>
#include <math/algebra/vector.h>

#ifdef _WIN32
#include <input/keyboard/keyboardInput.h>
using namespace rev::input;
#endif // _WIN32

using namespace std;
using namespace rev::math;

using namespace rev::gfx;

namespace rev::gfx {

	//----------------------------------------------------------------------------------------------
	ForwardPass::ForwardPass(gfx::Device& device, const math::Vec2u& viewportSize, gfx::FrameBuffer target)
		: m_gfxDevice(device)
	{
		loadCommonShaderCode();

		mEV = 1.0f;

		// Init graphics render pass
		RenderPass::Descriptor passDesc;
		passDesc.clearDepth = 1;
		passDesc.clearColor = { 0.f,0.f,0.f,1.f }; // ?89.f/255.f,235.f/255.f,1.f,1.f
		passDesc.clearFlags = RenderPass::Descriptor::Clear::All;
		passDesc.target = target;
		passDesc.viewportSize = viewportSize;
		m_pass = device.createRenderPass(passDesc);
		// Command buffer
		m_pass->record(m_drawCommands);

		// Common pipeline config
		m_commonPipelineDesc.cullBack = true;
		m_commonPipelineDesc.depthTest = Pipeline::Descriptor::DepthTest::Lequal;
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::loadCommonShaderCode()
	{
		ShaderProcessor::MetaData metadata;
		ShaderProcessor::loadCodeFromFile("forward.fx", mForwardShaderCommonCode, metadata);
		// TODO: Actualle use the metadata (unifrom layouts)
		for(auto& file : metadata.dependencies)
		{
			core::FileSystem::get()->onFileChanged(file) += [this](const char*) {
				mPipelines.clear();
				ShaderProcessor::MetaData metadata;
				mForwardShaderCommonCode.clear();
				ShaderProcessor::loadCodeFromFile("forward.fx", mForwardShaderCommonCode, metadata);
			};
		}
	}	

	//----------------------------------------------------------------------------------------------
	std::string ForwardPass::vertexFormatDefines(RenderGeom::VtxFormat vertexFormat)
	{
		// TODO: Create this defines procedurally with more information from the actual rendergeom
		string defines;
		if(vertexFormat.position() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_POSITION_FLOAT 0\n";
		if(vertexFormat.normal() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_NORMAL_FLOAT 1\n";
		if(vertexFormat.tangent() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_TANGENT_FLOAT 2\n";
		if(vertexFormat.uv() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_UV_FLOAT 3\n";
		return defines;
	}

	//----------------------------------------------------------------------------------------------
	gfx::Pipeline ForwardPass::getPipeline(const Material& mat, RenderGeom::VtxFormat vtxFormat, const EnvironmentProbe* env, bool shadows, bool mirror)
	{
		// Locate the proper pipeline set
		auto code = effectCode(mirror, env, shadows);
		auto setIter = mPipelines.find({code, &mat.effect()});
		if(setIter == mPipelines.end())
		{
			setIter = mPipelines.emplace(
				std::pair(code, &mat.effect()),
				PipelineSet()
			).first;
			auto& effect = mat.effect();
			// Listen to effect reload
			mat.effect().onReload([this, code](Effect& effect){
				auto setIter = mPipelines.find({code, &effect});
				if(setIter != mPipelines.end())
					setIter->second.clear();
			});
		}
		auto& pipelineSet = setIter->second;

		std::string environmentDefines = env ? "#define sampler2D_uEnvironment\n#define sampler2D_uIrradiance\n" : "";
		std::string shadowDefines = shadows ? "#define sampler2D_uShadowMap\n#define mat4_uMs2Shadow\n" : "";
		std::string skinningDefines = "";
		if(vtxFormat.weights() != RenderGeom::VtxFormat::Storage::None)
		{
			skinningDefines = "#define HW_SKINNING\n";
		}

		// Locate the proper shader in the set
		const auto& descriptor = std::pair(vtxFormat.code(),  mat.bakedOptions()); // TODO: Hash this once during material setup. Use hash for faster indexing. Maybe incorporate effect in the hash.
		auto iter = pipelineSet.find(descriptor);
		if(iter == pipelineSet.end())
		{
			Pipeline::ShaderModule::Descriptor stageDesc;
			stageDesc.code = {
				vertexFormatDefines(vtxFormat).c_str(),
				environmentDefines.c_str(),
				shadowDefines.c_str(),
				skinningDefines.c_str(),
				mat.bakedOptions().c_str(),
				mForwardShaderCommonCode.c_str(),
				mat.effect().code().c_str()
			};
			stageDesc.stage = Pipeline::ShaderModule::Descriptor::Vertex;
			m_commonPipelineDesc.vtxShader = m_gfxDevice.createShaderModule(stageDesc);
			stageDesc.stage = Pipeline::ShaderModule::Descriptor::Pixel;
			m_commonPipelineDesc.pxlShader = m_gfxDevice.createShaderModule(stageDesc);
			// Check against invalid pipeline code
			Pipeline pipeline;
			if(m_commonPipelineDesc.vtxShader.valid()
			&& m_commonPipelineDesc.pxlShader.valid())
			{
				m_commonPipelineDesc.frontFace = mirror ? Pipeline::Descriptor::Winding::CW : Pipeline::Descriptor::Winding::CCW;
				pipeline = m_gfxDevice.createPipeline(m_commonPipelineDesc);
			}

			iter = pipelineSet.emplace(
				descriptor,
				pipeline
			).first;
		}
		return iter->second;
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::render(
		const Camera& eye,
		const EnvironmentProbe* env,
		bool useShadows,
		const std::vector<gfx::RenderItem>& renderables,
		const CommandBuffer::UniformBucket& sharedUniforms)
	{
#ifdef _WIN32
		// Shader reload
		KeyboardInput* input = KeyboardInput::get();
		if (input->pressed('R'))
		{
			mPipelines.clear();
			loadCommonShaderCode();
		}
#endif

		// Compute global variables
		// Render
		resetStats();
		m_drawCommands.clear();

		CommandBuffer::UniformBucket uniforms;

		Mat44f wvp;
		float aspectRatio = float(m_viewportSize.x())/m_viewportSize.y();
		auto viewProj = eye.viewProj(aspectRatio);

		for(auto& renderable : renderables)
		{
			// Matrices
			Mat44f world = renderable.world.matrix();
			bool mirroredGeometry = affineTransformDeterminant(world) < 0.f;
			auto pipeline = getPipeline(renderable.material, renderable.geom.vertexFormat(), env, useShadows, mirroredGeometry);
			if(!pipeline.isValid())
				continue;

			uniforms.clear();
			wvp = viewProj * world;
			uniforms.addParam(0, wvp);
			uniforms.addParam(1, world);
			// Skinning matrix
			//if(_skin)
			//	mBackEnd.addParam(30, _skin->appliedPose);
			renderable.material.bindParams(uniforms);
			// Lighting
			uniforms.addParam(3, mEV); // Exposure value
			uniforms.addParam(4, eye.world().position());
			if(env)
			{
				uniforms.addParam(7, env->texture());
				uniforms.addParam(18, (float)env->numLevels());
			}

			m_drawCommands.setPipeline(pipeline);
			m_drawCommands.setUniformData(uniforms);
			m_drawCommands.setUniformData(sharedUniforms);
			m_drawCommands.setVertexData(renderable.geom.getVao());
			assert(renderable.geom.indices().componentType == GL_UNSIGNED_SHORT);
			m_drawCommands.drawTriangles(renderable.geom.indices().count, CommandBuffer::IndexType::U16);
		}

		// Finish pass
		m_gfxDevice.renderQueue().submitPass(*m_pass);
	}

	//----------------------------------------------------------------------------------------------
	void ForwardPass::resetStats(){
		m_numMeshes = 0;
		m_numRenderables = 0;
	}
}