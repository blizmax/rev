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

#include "fullScreenPass.h"

namespace rev::gfx {

	FullScreenPass::FullScreenPass(gfx::Device& device, const RenderPass::Descriptor& desc, const char* code)
		: m_device(device)
	{
		// Create a full screen quad
		m_quad = rev::gfx::RenderGeom::quad({2.f, 2.f});

		// Create pass
		m_pass = device.createRenderPass(desc);
		m_pass->record(m_commands);

		// Common pass code
		m_commonCode = R"(
#ifdef VTX_SHADER
	layout(location = 0) in vec3 vertex;

	void main ( void )
	{
		gl_Position = vec4(vertex.xy, 0.0, 1.0);
	}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;

vec3 shade();

void main (void) {	
	outColor = shade();
}
#endif
)";
		if(code)
		{
			// Initialize pipeline on start
			//setPassCode(code);
		}
	}

	void FullScreenPass::render(const CommandBuffer::UniformBucket& passUniforms)
	{
		if(!m_pipeline.isValid())
			return;

		assert(m_pass);
		m_commands.clear();
		m_commands.setPipeline(m_pipeline);
		m_commands.setUniformData(passUniforms);
		m_commands.setVertexData(m_quad.getVao());
		m_commands.drawTriangles(6, CommandBuffer::IndexType::U16);
	}

	void FullScreenPass::submit() const
	{
		assert(m_pass);
		m_device.renderQueue().submitPass(*m_pass);
	}

}
