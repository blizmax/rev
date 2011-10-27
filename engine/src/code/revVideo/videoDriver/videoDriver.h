////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// Video driver interface

#ifndef _REV_REVVIDEO_DRIVER_DRIVER_H_
#define _REV_REVVIDEO_DRIVER_DRIVER_H_

#include "revCore/math/matrix.h"

namespace rev	{

	namespace	video
{
	// Forward declarations
	class CColor;
	class CShader;

	class IVideoDriver
	{
	public:
		enum EAttribType
		{
			eVertex = 0,
			eColor,

			eAttribCount
		};

		enum EUniform
		{
			eMVP = 0,	// Model-view-projection matrix

			eUniformCount
		};
	public:
		// Virtual destructor
		virtual ~IVideoDriver() {}
		// ----- Per frame tasks ---- //
		virtual	void	beginFrame	() = 0;
		virtual	void	endFrame	() = 0;

		// ---- Accessor methods ---- //
		virtual void	setBackgroundColor	(const CColor& _color) = 0;
		virtual void	setScreenSize		(const unsigned int _width,
											const unsigned int _height) = 0;

		// ---- Render related ---- //
		virtual void	setShader			(const int _shader) = 0;
		virtual int		getUniformId		(const char * _name) const = 0;
		virtual void	setRealAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer) = 0;
		//virtual void	setIntAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer) = 0;
		//virtual void	setUniform		(const int _uniformId, const TReal _value) = 0;
		virtual	void	setUniform			(EUniform _id, const CMat4& _value) = 0;
		virtual void	drawIndexBuffer		(const int _nIndices, const unsigned short * _indices, const bool _strip) = 0;

		// --- Shader management -- //
		int 	getShader		(const char * _vtxShaderName, const char * _pxlShadeName);
		void	releaseShader	(const int _shader);

	private:
		virtual int 	loadShader		(const char * _vtxName, const char * _pxlName) = 0;
		virtual	void	deleteShader	(const int _id) = 0;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_DRIVER_DRIVER_H_