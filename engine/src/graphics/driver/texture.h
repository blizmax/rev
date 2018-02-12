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

#include <graphics/driver/openGL/openGL.h>
#include <graphics/Image.h>
#include <string>

namespace rev { namespace graphics {

	class Texture
	{
	public:
		std::string name;

		Texture(const ImageRGB8& image)
		{
			glGenTextures(1, &mGLName);
			glBindTexture(GL_TEXTURE_2D, mGLName);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.size().x(), image.size().y(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		~Texture()
		{
			glDeleteTextures(1, &mGLName);
		}

		static std::shared_ptr<Texture> load(const std::string _name)
		{
			auto img = ImageRGB8::load(_name);
			if(!img)
				return nullptr;
			auto tex = std::make_shared<Texture>(*img);
			tex->name = _name;
			return tex;
		}

		// Accessors
		auto glName() const { return mGLName; }

	private:
		GLuint mGLName = 0;
	};

}}
