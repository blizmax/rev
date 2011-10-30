////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// plain texture material

#include "plainTextureMaterial.h"

#include "revVideo/texture/texture.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CPlainTextureMaterial::CPlainTextureMaterial(CTexture * _texture)
		:mTexture(_texture)
	{
		if(0 == mShader)
			mShader = CPxlShader::manager()->get("plainTexture.pxl");
		else
			mShader->get();
		if(0 != _texture)
			_texture->get();
	}

	//------------------------------------------------------------------------------------------------------------------
	CPlainTextureMaterial::~CPlainTextureMaterial()
	{
		CPxlShader::manager()->release(mShader);
		if(0 != mTexture)
			CTexture::manager()->release(mTexture);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPlainTextureMaterial::setTexture(CTexture * _texture)
	{
		if(0 != mTexture)
			CTexture::manager()->release(mTexture);
		_texture->get();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPlainTextureMaterial::setEnviroment() const
	{
		IVideoDriver * driver = SVideo::get()->driver();
		int textureUniformId = driver->getUniformId("texture");
		driver->setUniform(textureUniformId, 0, mTexture); // Set my texture into texture slot 0
	}
}	// namespace video
}	// namespace rev
