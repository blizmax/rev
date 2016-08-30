//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures
#ifndef _REV_VIDEO_BASICTYPES_TEXTURE_H_
#define _REV_VIDEO_BASICTYPES_TEXTURE_H_

#include <math/algebra/vector.h>
#include <string>


namespace rev {
	namespace video {

		class GraphicsDriver;

		class Texture {
		public:
			enum class EImageFormat {
				rgb,
				rgba,
				alpha,
				luminance,
				lumiAlpha,
				depth,
				depthStencil
			};

			enum class EByteFormat {
				eUnsignedByte,
				eByte,
				eUnsignedShort,
				eShort,
				eUnsignedInt,
				eInt,
				eFloat
			};

			math::Vec2u size;
			void * data;
			EImageFormat imgFormat;
			EByteFormat byteFormat;

			static Texture* load(const std::string& _fileName, GraphicsDriver* _driver);
		};
	}
}

#endif // _REV_VIDEO_BASICTYPES_TEXTURE_H_
