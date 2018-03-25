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
#include <graphics/driver/texture.h>
#include <math/algebra/vector.h>
#include <memory>
#include <string>
#include <vector>
#include "Effect.h"

namespace rev { namespace graphics {

	class Material
	{
	public:
		std::string shader;

		void addParam(GLint pos,float f);
		void addParam(GLint pos, const math::Vec3f& v);
		void addParam(GLint pos, const math::Vec4f& v);
		void addTexture(GLint pos, std::shared_ptr<const Texture> t);

	private:
		using TexturePtr = std::shared_ptr<const Texture>;

		std::vector<std::pair<GLint,float>>			mFloatParams;
		std::vector<std::pair<GLint,math::Vec3f>>	mVec3fParams;
		std::vector<std::pair<GLint,math::Vec4f>>	mVec4fParams;
		std::vector<std::pair<GLint,TexturePtr>>	mTextureParams;
	};

}}
