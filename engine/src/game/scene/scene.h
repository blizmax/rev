//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple scene manager
#ifndef _REV_GAME_SCENE_SCENE_H_
#define _REV_GAME_SCENE_SCENE_H_

#include <core/components/sceneNode.h>
#include <video/graphics/renderer/renderContext.h>
#include <core/components/sceneNode.h>
#include <map>
#include <string>

namespace rev {
	namespace game {
		
		class Scene {
		public:
			/// Open a scene and return it as a vector of scene nodes.
			/// \return true on succuess importing the scene
			static Scene* import(const char* _fileName);

			video::RenderContext*			mRenderContext;
			std::map<std::string,core::SceneNode*>	mSceneGraph;
		};

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_SCENE_H_