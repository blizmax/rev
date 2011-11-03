////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model instance

#include "staticModelInstance.h"
#include "staticModel.h"

#include "revCore/node/node.h"
#include "revVideo/camera/camera.h"
#include "revVideo/scene/renderableInstance.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::CStaticModelInstance(const char * _modelName, IMaterialInstance * _material)
	{
		mModel = CStaticModel::manager()->get(string(_modelName));
		IRenderableInstance::setRenderable(mModel);
		mMaterialInstance = _material;
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::~CStaticModelInstance()
	{
		CStaticModel::manager()->release(mModel);
	}

}	// namespace video
}	// namespace rev