//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

#include <engine.h>
#include <game/geometry/procedural/basic.h>
#include <input/keyboard/keyboardInput.h>
#include <math/algebra/vector.h>
#include <util/app/app3d.h>
#include <video/basicTypes/color.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderContext.h>
#include <video/graphics/renderer/material.h>
#include <video/graphics/renderer/renderMesh.h>
#include <video/graphics/renderer/renderObj.h>
#include <core/memory/newAllocator.h>
#include <core/components/sceneNode.h>
#include <game/physics/rigidBody.h>
#include <game/physics/physicsWorld.h>
#include <game/scene/scene.h>
#include <vector>
#include <iostream>
#include "player.h"

using namespace rev::core;
using namespace rev::game;
using namespace rev::math;
using namespace rev::video;

class RacingDemo : public rev::App3d {
public:
	RacingDemo(int _argc, const char** _argv)
		: App3d(_argc, _argv)
	{
		processArgs(_argc, _argv);

		// Construct global objects
		mRenderer = new ForwardRenderer();
		mRenderer->init<NewAllocator>(&driver3d(), mAlloc);
		mRenderer->setWindowSize(window().size());
		mWorld = new PhysicsWorld();
		mScene = new Scene();
		mPlayer = new Player(mWorld);
		mPlayer->body()->setPosition({0.f,0.f,3.f});
		mScene->mRenderContext->insert(mPlayer->renderObj());
		mScene->mRenderContext->setCamera(mPlayer->camera());

		Vec3f groundSize = {1000.f, 1000.f, 2.f};
		RigidBody* groundBd = RigidBody::box(0.f, groundSize);
		RenderObj* groundRo = new RenderObj(Procedural::box(groundSize));
		SceneNode* ground = new SceneNode;
		ground->attachTo(groundBd);
		groundRo->attachTo(ground);
		groundBd->setPosition({0.f,0.f,-groundSize.z*0.5f});
		mWorld->addRigidBody(groundBd);
		mScene->mRenderContext->insert(groundRo);

		// Obstacles
		for(auto i = 0; i < 10; ++i)
			createCube({-10.f+4.f*i, 20.f, 0.5f}, mWorld, mScene->mRenderContext);
	}

	~RacingDemo() {
		mRenderer->end<NewAllocator>(mAlloc);
	}

	Player* mPlayer;
	ForwardRenderer* mRenderer;
	Scene* mScene;
	NewAllocator mAlloc;
	PhysicsWorld* mWorld;
	float t = 0;

private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		_argc;
		_argv;
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {
		t += _dt;
		mPlayer->update(_dt);
		mWorld->simulate(_dt);

		mRenderer->renderContext(*mScene->mRenderContext);
		return true;
	}

	//----------------------------------------------------------------
	void createCube(const Vec3f& _pos, PhysicsWorld* _w, RenderContext* _renderCtxt) {
		SceneNode* node = new SceneNode();
		Vec3f size = Vec3f(1.f);
		RigidBody* bd = RigidBody::box(0.f, size);
		_w->addRigidBody(bd);
		RenderObj* model = new RenderObj(Procedural::box(size));
		_renderCtxt->insert(model);
		model->attachTo(node);
		node->attachTo(bd);
		bd->setPosition(_pos);
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	RacingDemo app(_argc,_argv);

	while(app.update());
	return 0;
}