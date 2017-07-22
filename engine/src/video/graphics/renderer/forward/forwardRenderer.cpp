//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/15
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#include "forwardRenderer.h"

#ifdef OPENGL_45
#include <core/components/affineTransform.h>
#include "../backend/rendererBackEnd.h"
#include <video/basicTypes/camera.h>
#include <video/graphics/renderScene.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/staticRenderMesh.h>
#include <video/graphics/material.h>

using namespace rev::math;

#endif // OPENGL_45

#ifdef REV_USE_VULKAN
#include <iostream>
#include <video/graphics/driver/graphicsDriver.h>
#include <core/platform/fileSystem/file.h>

using namespace std;

#endif // REV_USE_VULKAN

namespace rev {
	namespace video {

		namespace { // Vulkan utilities
			VkShaderModule createShaderModule(VkDevice device, const core::File& code) {
				// Ensure code is aligned before we recast
				std::vector<char> alignedCode(code.sizeInBytes());
				for(size_t i = 0; i < code.sizeInBytes(); ++i) {
					alignedCode[i] = code.bufferAsText()[i];
				}

				VkShaderModuleCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = alignedCode.size();
				createInfo.pCode = reinterpret_cast<const uint32_t*>(alignedCode.data());

				VkShaderModule shaderModule;
				if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
					cout << "failed to create shader module!\n";
					return VK_NULL_HANDLE;
				}

				return shaderModule;
			}
		}

#ifdef REV_USE_VULKAN
		ForwardRenderer::~ForwardRenderer() {
			if(!mDevice)
				return;

			if(mPipeline) {
				vkDestroyPipeline(mDevice, mPipeline, nullptr);
				vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
			}

			if(mDevice && mRenderPass)
				vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::init(const NativeFrameBuffer& _dstFrameBuffer) {
			mDevice = GraphicsDriver::get().device(); // Vulkan device
			assert(mDevice);

			if(!createRenderPass(_dstFrameBuffer)) {
				cout << "Forward renderer: failed to create render pass!\n";
				return false;
			}

			auto viewportSize = _dstFrameBuffer.size();
			if(!createPipeline({viewportSize.x, viewportSize.y })) {
				cout << "failed to create graphics pipeline!";
				return false;
			}

			return true;
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::createRenderPass(const NativeFrameBuffer& _dstFB) {
			const VkAttachmentDescription& colorAttachment = _dstFB.attachmentDescription();

			// Subpasses
			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;

			// Pass
			VkSubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = 1;
			renderPassInfo.pAttachments = &colorAttachment;
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			return VK_SUCCESS == vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &mRenderPass);
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::createPipeline(const VkExtent2D& _viewportSize) {
			// ----- Create a graphics pipeline -----
			auto vertexShader = core::File("data/vert.spv");
			auto pixelShader = core::File("data/frag.spv");

			VkShaderModule vertShaderModule = createShaderModule(mDevice, vertexShader);
			VkShaderModule fragShaderModule = createShaderModule(mDevice, pixelShader);
			// Vertex shader stage
			VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";
			// Fragment shader stage
			VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

			// Vertex input stage
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 0;
			vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
			vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

																	// Topology
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			// Viewport
			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)_viewportSize.width;
			viewport.height = (float) _viewportSize.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			// Scissor
			VkRect2D scissor = {};
			scissor.offset = {0, 0};
			scissor.extent = _viewportSize;

			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;

			// Rasterizer
			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f; // Optional
			rasterizer.depthBiasClamp = 0.0f; // Optional
			rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading = 1.0f; // Optional
			multisampling.pSampleMask = nullptr; // Optional
			multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
			multisampling.alphaToOneEnable = VK_FALSE; // Optional

													   // Color blending
			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f; // Optional
			colorBlending.blendConstants[1] = 0.0f; // Optional
			colorBlending.blendConstants[2] = 0.0f; // Optional
			colorBlending.blendConstants[3] = 0.0f; // Optional

													// Pipeline layout
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 0; // Optional
			pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
			pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
			pipelineLayoutInfo.pPushConstantRanges = 0; // Optional

			if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
				cout << "failed to create pipeline layout!\n";
				return false;
			}

			// Actual pipeline
			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;

			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = nullptr; // Optional
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = nullptr; // Optional

			pipelineInfo.layout = mPipelineLayout;

			pipelineInfo.renderPass = mRenderPass;
			pipelineInfo.subpass = 0;

			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1; // Optional

			bool ok = (VK_SUCCESS == vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline));

			vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
			vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);

			return ok;
		}

#endif // REV_USE_VULKAN

#ifdef OPENGL_45
		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::init(GraphicsDriver* _driver) {
			mDriver = _driver;
			mBackEnd = new RendererBackEnd(_driver);
			mProgram = Shader::manager()->get("data\\pbr");
			mSkyboxShader = Shader::manager()->get("data\\skybox");
			mSkybox = Texture::loadFromFile("data\\skybox.pvr");
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::render(const std::vector<RenderObj*>& _scene, const Camera& _cam) const {
			// TODO: ShadowPass?
			// TODO: Sort into render queues based on material
			// Set global uniforms
			mDriver->beginFrame();
			//mDriver->setRenderTarget(_rt);
			mDriver->setViewport(Vec2i(0, 0), Vec2u(800, 600));
			mDriver->clearZBuffer();
			mDriver->clearColorBuffer();
			// Render state info
			RendererBackEnd::DrawCall	draw;
			RendererBackEnd::DrawInfo&	drawInfo = draw.renderStateInfo;
			drawInfo.program = mProgram;
			Vec3f globalLightDir = (Vec3f(0.8f, -1.f, -0.9f)).normalized();
			Vec3f viewPos = _cam.node()->component<core::AffineTransform>()->position();
			drawInfo.lightClr = Vec3f(1.f, 1.f, 0.9f)*1000.f;
			// Camera
			Mat34f invView = _cam.view().inverse();
			Mat44f viewProj = _cam.projection() * invView;
			// render objects
			for (const auto obj : _scene) {
				Mat34f modelMatrix = obj->node()->component<core::AffineTransform>()->matrix();
				Mat34f invModelMtx = modelMatrix.inverse();

				drawInfo.wvp = viewProj * modelMatrix;
				drawInfo.lightDir = invModelMtx.rotate(globalLightDir);
				drawInfo.viewPos = invModelMtx * viewPos;

				if (obj->material) {
					for (const auto& m : obj->material->mMaps)
						drawInfo.texUniforms.push_back(m);
					if (mSkybox)
						drawInfo.texUniforms.push_back(std::make_pair("environmentMap", mSkybox));
					for (const auto& v3 : obj->material->mVec3s)
						drawInfo.vec3Uniforms.push_back(v3);
					for (const auto& f : obj->material->mFloats)
						drawInfo.floatUniforms.push_back(f);
				}

				draw.mesh = obj->mesh();
				mBackEnd->draw(draw);
				draw.renderStateInfo.texUniforms.clear();
				draw.renderStateInfo.vec3Uniforms.clear();
				draw.renderStateInfo.floatUniforms.clear();
			}
			// Draw skybox cubemap
			drawSkyboxCubemap(_cam);
			mBackEnd->flush();
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::drawSkyboxCubemap(const Camera& _cam) const {
			RendererBackEnd::DrawInfo cubeMapInfo;
			cubeMapInfo.program = mSkyboxShader;
			cubeMapInfo.texUniforms.push_back(std::make_pair("albedo", mSkybox));
			Mat34f centeredView = _cam.view();
			centeredView.setCol(3, Vec3f(0));
			cubeMapInfo.wvp = _cam.projection() * centeredView.inverse();
			mBackEnd->drawSkybox(cubeMapInfo);
		}

		//--------------------------------------------------------------------------------------------------------------
/*		void ForwardRenderer::init(GraphicsDriver* _driver) {
			mDriver = _driver;
			mBackEnd = new RendererBackEnd(mDriver);
			mShader = Shader::manager()->get("forward");
			//mSkyShader = Shader::manager()->get("skybox");
			mDebug = new DebugDrawer(mDriver, mBackEnd);

			mShadowPass[0] = new ShadowPass(mDriver, mBackEnd, 1024);
			mShadowPass[1] = new ShadowPass(mDriver, mBackEnd, 1024);
			mShadowPass[2] = new ShadowPass(mDriver, mBackEnd, 1024);

			mDriver->setShader(mShader);
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::end() {
			delete mShadowPass[0];
			delete mShadowPass[1];
			delete mShadowPass[2];
			delete mBackEnd;
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::startFrame()
		{
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::finishFrame()
		{
			mDebug->drawBasis(Mat34f::identity());
			mDebug->render();
			//mDriver->finishFrame();
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::setShadowLight(const math::Vec3f& _pos, const math::Vec3f& dir, const Color& _color) {

		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::renderContext(const RenderContext& _context) {
			auto camView = _context.camera()->view();
			auto pov = (mDebugCamera?mDebugCamera:_context.camera())->view();
			// Render shadow pass
			/*Frustum globalFrustum = adjustShadowFrustum(_context);
			float maxFar = globalFrustum.farPlane();
			float farPlane[3] = {maxFar*0.1f, maxFar * 0.3f, maxFar };
			for(size_t i = 0; i < 3; ++i) {
				Frustum cascadeFrustum = Frustum(globalFrustum.aspectRatio(), globalFrustum.fov(), globalFrustum.nearPlane(), farPlane[i]);
				mShadowPass[i]->config(_context.lightDir(), camView, cascadeFrustum, maxFar);

				for (auto obj : _context) {
					if(obj->castShadows)
						renderObject(*obj);
				}
				mShadowPass[i]->finishPass();
				//mShadowPass[i]->tex()->generateMipMaps();
			}
			//mDriver->finishFrame();
			if (mDebugCamera) {
				for (size_t i = 0; i < 3; ++i)
					mShadowPass[i]->mDebug = mDebug;
				mDebug->drawLine(camView.col(3), camView*Vec3f(0.f,globalFrustum.farPlane(),0.f), Color(1.f,0.f,1.f));
				mDebug->setViewProj(pov, mDebugCamera->projection());
				mDebug->drawBasis(camView);
				mDebug->drawFrustum(camView, globalFrustum, Color(1.f,0.f,1.f));
			}
			else {
				for (size_t i = 0; i < 3; ++i)
					mShadowPass[i]->mDebug = nullptr;
				mDebug->setViewProj(pov, _context.camera()->projection());
			}*/
			// Render pass
			//mDriver->setMultiSampling(false);
			//mDriver->setRenderTarget(nullptr);
			//mDriver->setViewport(math::Vec2i(0, 0), mWindowSize);
			// Render skybox
/*			if (_context.skyBox) {
				mDriver->setZCompare(false); // Use the skybox to clear the buffer
				mDriver->setShader(mSkyShader);
				int uMap = mDriver->getUniformLocation("uSkyMap");
//				mDriver->setUniform(uMap, _context.skyBox->mMaterial->mDiffMap);
				Mat34f centeredView = pov;
				centeredView.setCol(3, Vec3f::zero());
				if (mDebugCamera)
					mBackEnd->setCamera(centeredView, mDebugCamera->projection());
				else
					mBackEnd->setCamera(centeredView, _context.camera()->projection());
				renderObject(*_context.skyBox);
				mDriver->setZCompare(true);
			} else {*/
			//mDriver->setClearColor(Color(0.7f, 0.8f, 1.f, 1.f));
			//mDriver->clearColorBuffer();
		//}
		/*mDriver->clearZBuffer();
		mDriver->setCulling(GraphicsDriver::ECulling::eBack);
		mBackEnd->setShadowVp(0, mShadowPass[0]->viewProj());
		mBackEnd->setShadowVp(1, mShadowPass[1]->viewProj());
		mBackEnd->setShadowVp(2, mShadowPass[2]->viewProj());
		mBackEnd->setShader(mShader);
		int uLightDir = mDriver->getUniformLocation("uLightDir");
		mDriver->setUniform(uLightDir, _context.lightDir());
		int uShadowMap0 = mDriver->getUniformLocation("uShadowMap0");
		mDriver->setUniform(uShadowMap0, mShadowPass[0]->tex());
		int uShadowMap1 = mDriver->getUniformLocation("uShadowMap1");
		mDriver->setUniform(uShadowMap1, mShadowPass[1]->tex());
		int uShadowMap2 = mDriver->getUniformLocation("uShadowMap2");
		mDriver->setUniform(uShadowMap2, mShadowPass[2]->tex());
		//int uFarShadowMap = mDriver->getUniformLocation("uFarShadowMap");
		//mDriver->setUniform(uFarShadowMap, mFarShadowPass->tex());
		if(mDebugCamera)
			mBackEnd->setCamera(pov, mDebugCamera->projection());
		else
			mBackEnd->setCamera(pov, _context.camera()->projection());
		// Render all objects
		for (auto obj : _context) {
			renderObject(*obj);
		}
	}

	//--------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::renderObject(const RenderObj& _obj)
	{
		RendererBackEnd::StaticGeometry geom;
		if (_obj.mesh()->stripLength > 0) {
			geom.strip = true;
			geom.indices = _obj.mesh()->triStrip;
			geom.nIndices = _obj.mesh()->stripLength;
		} else {
			geom.strip = false;
			geom.indices = _obj.mesh()->indices;
			geom.nIndices = _obj.mesh()->nIndices;
		}
		geom.vertices = _obj.mesh()->vertices;
		geom.normals = _obj.mesh()->normals;
		geom.uvs = _obj.mesh()->uvs;
		geom.nVertices = _obj.mesh()->nVertices;
		geom.transform = _obj.transform();
		//if (_obj.mMaterial)
		//	geom.color = _obj.mMaterial->mDiffuse;
		//else
			geom.color = Color(1.f, 1.f, 1.f);
		mBackEnd->render(geom);
	}

	//--------------------------------------------------------------------------------------------------------------
	Frustum ForwardRenderer::adjustShadowFrustum(const RenderContext& _context) {
		Mat34f invView;
		_context.camera()->view().inverse(invView);
		Frustum camF = _context.camera()->frustum();
		float minCaster = camF.farPlane();
		float maxCaster = camF.nearPlane();
		for (auto obj : _context) {
			if (obj->castShadows)
			{
				float objDist = (invView*obj->node()->position()).y;
				// Contribute to frustum adjustment
				minCaster = std::min(minCaster, objDist - obj->mBBox.radius());
				maxCaster = std::max(maxCaster, objDist + obj->mBBox.radius());
			}
		}
		float adjNear = std::max(camF.nearPlane(), minCaster);
		float adjFar = std::min(camF.farPlane(), maxCaster);
		if(adjFar < adjNear) // No casters in the frustum
			adjFar = adjNear;
		return Frustum(camF.aspectRatio(), camF.fov(), adjNear, adjFar);
	}*/
#endif // OPENGL_45
	}
}
