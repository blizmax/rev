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

#include "../renderPass.h"
#include "openGL.h"
#include <vector>
#include <cstdint>

namespace rev :: gfx
{
	class RenderQueueOpenGL;

	class RenderPassOpenGL : public RenderPass
	{
	public:
		RenderPassOpenGL(const Descriptor& desc, int numDrawBuffers, std::int32_t id);

		void reset() override;
		void setViewport(const math::Vec2u& start, const math::Vec2u& size) override;
		void record(const CommandBuffer&) override;

		// OpenGL specific
		void bindTo(RenderQueueOpenGL& renderQueue) const;
		void submit(RenderQueueOpenGL& renderQueue) const;

	private:
		Descriptor m_desc;
		std::vector<const CommandBuffer*> m_commandList;
		std::vector<GLenum> m_colorAttachs;
	};
}