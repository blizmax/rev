//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "editor/editor.h"
#include <game/scene/scene.h>
#include <game/scene/ComponentLoader.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/renderer/ForwardRenderer.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderScene.h>

using namespace rev::editor;

namespace rev {

	class Player {
	public:
		Player()
		{}

		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		typedef rev::graphics::GraphicsDriverGL::NativeWindow	Window;
		bool init(Window);
		void loadScene(const std::string& _assetFileName);

		void onWindowResize(const math::Vec2u& _newSize);

		// Common components
		bool update();

	private:
		void createCamera();

		game::ComponentLoader					mComponentFactory;
		graphics::RenderScene					mGraphicsScene;
		game::Scene								mGameScene;
		const graphics::Camera*					mCamera;
		graphics::ForwardRenderer				mRenderer;
		graphics::GraphicsDriverGL*				mGfxDriver = nullptr;
		Editor									mGameEditor;
	};

}	// namespace rev