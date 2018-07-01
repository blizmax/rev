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

#include "renderGeom.h"

namespace rev::graphics
{

	//----------------------------------------------------------------------------------------------
	RenderGeom::VtxFormat::VtxFormat(Storage pos, Storage nrm, Storage tan, Storage uv)
	{
		m_pos = pos;
		m_normal = nrm;
		m_tangent = tan;
		m_uv = uv;
	}

	//----------------------------------------------------------------------------------------------
	uint32_t RenderGeom::VtxFormat::code() const
	{
		return uint32_t(m_pos) << 24
			|| uint32_t(m_normal) << 16
			|| uint32_t(m_tangent) << 8
			|| uint32_t(m_uv);
	}

	//----------------------------------------------------------------------------------------------
	RenderGeom::RenderGeom(
		const Attribute* indices,
		const Attribute* position,
		const Attribute* normal,
		const Attribute* tangent,
		const Attribute* uv0)
	{
		assert(indices);
		assert(indices->componentType == GL_UNSIGNED_SHORT || indices->componentType == GL_UNSIGNED_BYTE);
		m_indices = *indices;
		assert(position);
		m_vtxAttributes.emplace_back(0, *position);
		m_vtxFormat = VtxFormat(
			VtxFormat::Storage::Float32,
			normal?VtxFormat::Storage::Float32 : VtxFormat::Storage::None,
			tangent?VtxFormat::Storage::Float32 : VtxFormat::Storage::None,
			uv0?VtxFormat::Storage::Float32 : VtxFormat::Storage::None
		);
		if(normal)
			m_vtxAttributes.emplace_back(1, *normal);
		if(tangent)
			m_vtxAttributes.emplace_back(2, *tangent);
		if(uv0)
			m_vtxAttributes.emplace_back(3, *uv0);

		m_bbox = position->bounds;

		initOpenGL();
	}

	//----------------------------------------------------------------------------------------------
	void RenderGeom::initOpenGL() {

		// VAO
		glGenVertexArrays(1,&m_vao);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices.bufferView->vbo);

		// Attributes
		for(auto& [ndx, attribute] : m_vtxAttributes)
		{
			glBindBuffer(GL_ARRAY_BUFFER, attribute.bufferView->vbo);
			glVertexAttribPointer(ndx, attribute.nComponents, attribute.componentType, attribute.normalized, attribute.stride, attribute.offset);
			glEnableVertexAttribArray(ndx); // Vertex pos
		}

		// Unbind VAO
		glBindVertexArray(0);
	}

	//----------------------------------------------------------------------------------------------
	/*
	static RenderGeom quad(const math::Vec2f& size)
	{
	auto half_x = 0.5f*size.x();
	auto half_y = 0.5f*size.y();
	std::vector<uint16_t> indices = { 0, 1, 2, 2, 1, 3};
	std::vector<Vertex> vertexData = {
	{{-half_x, -half_y, 0.f }, { 1.f, 0.f, 0.f }, {0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f }, { 0.f, 0.f }},
	{{ half_x, -half_y, 0.f }, { 1.f, 0.f, 0.f }, {0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f }, { 1.f, 0.f }},
	{{-half_x,  half_y, 0.f }, { 1.f, 0.f, 0.f }, {0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f }, { 0.f, 1.f }},
	{{ half_x,  half_y, 0.f }, { 1.f, 0.f, 0.f }, {0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f }, { 1.f, 1.f }}
	};

	return RenderGeom(vertexData, indices);
	}*/
}