////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on November 4th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Solid color material

#include "raymarchingMaterial.h"

#include "revCore/resourceManager/resourceManager.h"
#include "revCore/codeTools/log/log.h"
#include "revCore/time/time.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CRaymarchingMaterial::CRaymarchingMaterial():mTime(0.f)
	{
		mShader = CPxlShader::manager()->get("raymarching.pxl");
	}

	//------------------------------------------------------------------------------------------------------------------
	CRaymarchingMaterial::~CRaymarchingMaterial()
	{
		CPxlShader::manager()->release(mShader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CRaymarchingMaterial::setEnvironment() const
	{
		float dt = STime::get()->frameTime();
		mTime +=dt;
		IVideoDriver * driver = SVideo::get()->driver();
		int uniformId = driver->getUniformId("time");
		driver->setUniform(uniformId, mTime);
	}

}	// namespace video
}	// namespace rev
