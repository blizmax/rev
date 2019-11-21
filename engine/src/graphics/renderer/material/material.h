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
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/texture2d.h>
#include <math/algebra/vector.h>
#include <memory>
#include <string>
#include <vector>
#include "Effect.h"

namespace rev::gfx {

	class Material
	{
	public:
		// A material parameter will only be bound in when it shares
		// binding flags with those passed to the bindParams method
		enum BindingFlags
		{
			None = 0,
			Normals = 1<<0,
			PBR = 1<<1,
			Emissive = 1<<2,
			Opacity = 1<<3,
			All = 0xff
		};

		enum Alpha
		{
			opaque,
			mask,
			blend
		};

		Effect& effect() const { return *mEffect;}

		Material(const std::shared_ptr<Effect>& effect, Alpha = Alpha::opaque);
		Alpha alpha() const { return mAlpha; }
		bool isEmissive() const { return mIsEmissive; }

		// New params can only be added to the material before calling init
		void addParam(const std::string& name,float f, BindingFlags);
		void addParam(const std::string& name, const math::Vec3f& v, BindingFlags);
		void addParam(const std::string& name, const math::Vec4f& v, BindingFlags);
		void addTexture(const std::string& name, gfx::Texture2d t, BindingFlags);

		const std::string& bakedOptions() const { return mShaderOptionsCode; }
		void bindParams(gfx::CommandBuffer::UniformBucket& renderer, BindingFlags) const;

	private:
		const std::shared_ptr<Effect> mEffect;
		Alpha mAlpha = Alpha::opaque;
		bool mIsEmissive = false;
		std::string mShaderOptionsCode;

		template<class T> struct Param
		{
			GLint location;
			T value;
			BindingFlags flags;
		};

		std::vector<Param<float>>		mFloatParams;
		std::vector<Param<math::Vec3f>>	mVec3fParams;
		std::vector<Param<math::Vec4f>>	mVec4fParams;
		std::vector<Param<Texture2d>>	mTextureParams;
	};

}
