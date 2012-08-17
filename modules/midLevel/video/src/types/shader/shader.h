//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Shader

#ifndef _REV_VIDEO_TYPES_SHADER_SHADER_H_
#define _REV_VIDEO_TYPES_SHADER_SHADER_H_

namespace rev { namespace video
{
	// Forward declarations
	class PxlShader;
	class VtxShader;

	class Shader
	{
	public:
		Shader(const VtxShader*, const PxlShader*) {}
		virtual ~Shader() {} = 0;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_SHADER_SHADER_H_
