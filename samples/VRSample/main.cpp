//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

#include <util/app/app3d.h>
#include <core/components/sceneNode.h>
#include <game/geometry/procedural/basic.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/renderScene.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderContext.h>
#include <video/window/window.h>

using namespace rev::core;
using namespace rev::game;
using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

class VRSample : public rev::App3d {
public:
	VRSample(int _argc, const char** _argv)
		: App3d(_argc, _argv)
	{
		processArgs(_argc, _argv);
		mBackEnd = new RendererBackEnd(&driver3d());

		// --- Create basic game objects
		RenderMesh* cube = Procedural::box(Vec3f(1.f));
		cubeObj = new RenderObj(cube);
		renderScene.objects.push_back(cubeObj);
		// Camera
		cam = new Camera(1.5f, 0.1f, 1000.f);
		camera.addComponent(cam);
		camera.setPos(Vec3f(0.f, 0.f, -10.f));
		// World
		world.addComponent(cubeObj);
		world.setPos(Vec3f(0.f, 1.f, 0.f));

		program = Shader::manager()->get("shader");
	}

	~VRSample() {
	}

	RendererBackEnd* mBackEnd;
	RenderObj* cubeObj;
	Camera* cam;
	SceneNode camera;
	SceneNode world;
	RenderScene renderScene;
	Shader::Ptr	program;

	// Temp stuff

private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		_argc;
		_argv;
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {
		GraphicsDriver* driver = &driver3d();
		// start frame
		driver->setViewport(Vec2i(0,0), window().size());
		driver->clearZBuffer();
		driver->clearColorBuffer();
		driver->setShader(program);
		driver->setAttribBuffer(0, cubeObj->mesh()->nVertices, cubeObj->mesh()->vertices);
		driver->drawIndexBuffer(cubeObj->mesh()->nIndices, cubeObj->mesh()->indices, GraphicsDriver::EPrimitiveType::triangles);
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	VRSample app(_argc,_argv);

	while(app.update());
	return 0;
}