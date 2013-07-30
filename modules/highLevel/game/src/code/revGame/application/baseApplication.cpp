//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Basic application using rev game

#include "baseApplication.h"

#include <revCore/time/time.h>
#include <revGame/application/platform/osHandler.h>
#include <revGraphics3d/renderable/renderScene.h>
#include <revGraphics3d/renderer/forward/forwardRenderer.h>
#include <revInput/keyboard/keyboardInput.h>
#include <revMath/algebra/vector.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/color/color.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev::graphics3d;
using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	BaseApplication::BaseApplication()
		:mCamera(nullptr)
	{
		Time::init();
		KeyboardInput::init();
		VideoDriver::startUp();
		mVideoDriver = VideoDriver::get();
		mVideoDriver->createMainWindow();
		mVideoDriver->init3d();
		mDriver3d = VideoDriver::getDriver3d();
		mDriver3d->setClearColor(Color::Color(0.2f));
		mRenderer = new ForwardRenderer;

		Time::get()->update();
	}

	//------------------------------------------------------------------------------------------------------------------
	BaseApplication::~BaseApplication()
	{
		VideoDriver::shutDown();
	}

	//------------------------------------------------------------------------------------------------------------------
	void BaseApplication::run()
	{
		for(;;) {
			if(!OSHandler::get()->update())
				return;
			KeyboardInput::get()->refresh();
			if(!update())
				return;
			if(!render())
				return;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool BaseApplication::render()
	{
		mDriver3d->clearColorBuffer();
		mDriver3d->clearZBuffer();
		mDriver3d->setViewport(Vec2i::zero(), Vec2u(640, 480));
		if(nullptr != mCamera)
			mRenderer->render(*mCamera, *RenderScene::get());
		mDriver3d->finishFrame();
		return mVideoDriver->update();
	}

}	// namespace game
}	// namespace rev