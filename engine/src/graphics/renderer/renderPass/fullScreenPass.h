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
#pragma once

#include <graphics/backend/device.h>
#include <graphics/backend/frameBuffer.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/scene/renderGeom.h>

namespace rev::gfx {

	class FullScreenPass
	{
	public:
		FullScreenPass(gfx::Device&, const RenderPass::Descriptor& desc, const char* code = nullptr);

		void setPassCode(const char* code);

		void render(const CommandBuffer::UniformBucket& passUniforms);
		void submit() const;

		bool isOk() const { return m_pipeline.isValid(); }
		void onResizeTarget(const math::Vec2u& newSize)
		{
			m_pass->setViewport(math::Vec2u::zero(), newSize);
		}

	private:
		gfx::Device& m_device;
		RenderGeom m_quad;

		std::string m_commonCode;
		Pipeline m_pipeline;

		CommandBuffer m_commands;
		RenderPass* m_pass = nullptr;
	};

}