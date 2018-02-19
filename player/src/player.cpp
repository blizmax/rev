//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "player.h"
#include <math/algebra/vector.h>
#include <core/platform/fileSystem/file.h>
#include <core/time/time.h>
#include <core/tools/log.h>
#include <game/geometry/modelAsset.h>
#include <game/scene/camera.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/transform/transform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/scene/material.h>
#include <graphics/scene/renderGeom.h>

using namespace rev::math;
using namespace rev::graphics;
using namespace rev::game;

using Json = rev::core::Json;

namespace rev {

	const std::vector<Vec3f> vertices = {
		{1.f, 0.f, 1.f},
		{-1.f, 0.f, 1.f},
		{0.f,0.f, -1.f}
	};
	const std::vector<uint16_t> indices = { 0, 1, 2};

	namespace { // Anonymous namespace for temporary implementation of gltf loader

		namespace gltf {
			struct Buffer
			{
				Buffer(const std::string& assetsFolder, const Json& desc)
				{
					size_t length = desc["byteLength"];
					raw.resize(length);
					core::File bufferFile(assetsFolder + desc["uri"].get<std::string>());
					// TODO: Error checking with buffer size
					memcpy(raw.data(), bufferFile.buffer(), length);
				}

				std::vector<uint8_t> raw;
			};

			struct BufferView
			{
				uint8_t* data = nullptr;
				GLenum target = 0;
				size_t size = 0;
				size_t stride = 0;
			};

			struct Accessor
			{
				std::string type;

				enum class ComponentType : uint32_t
				{
					BYTE = 5120,
					UNSIGNED_BYTE = 5121,
					SHORT = 5122,
					UNSIGNED_SHORT = 5123,
					UNSIGNED_INT = 5125,
					FLOAT = 5126
				} componentType;

				uint8_t* data = nullptr;
				size_t count = 0;
			};

		}

		std::shared_ptr<SceneNode> loadGLTFScene(const std::string& assetsFolder)
		{
			Json sceneDesc;
			std::ifstream("helmet/damagedHelmet.gltf") >> sceneDesc;
			auto asset = sceneDesc.find("asset");
			if(asset == sceneDesc.end())
				return nullptr;
			if(asset.value()["version"] != "2.0")
				return nullptr;
			auto scene = sceneDesc.find("scene");
			auto scenesDict = sceneDesc.find("scenes");
			if(scenesDict == sceneDesc.end() || scene == sceneDesc.end() || scenesDict.value().size() == 0)
				return nullptr;
			// Load buffers
			std::vector<gltf::Buffer> buffers;
			for(auto& buffDesc : sceneDesc["buffers"])
				buffers.emplace_back(assetsFolder, buffDesc);

			// Load buffer views
			std::vector<gltf::BufferView> bufferViews;
			for(auto& viewDesc : sceneDesc["bufferViews"])
			{
				gltf::BufferView view;
				size_t offset = viewDesc["byteOffset"];
				size_t bufferNdx = viewDesc["buffer"];
				view.data = &buffers[bufferNdx].raw[offset];
				bufferViews.push_back(view);
			}

			// Load accessors
			std::vector<gltf::Accessor> accessors;
			for(auto& accessorDesc : sceneDesc["accessors"])
			{
				gltf::Accessor accessor;
				accessor.type = accessorDesc["type"].get<std::string>();
				unsigned cTypeDesc = accessorDesc["componentType"];
				accessor.componentType = gltf::Accessor::ComponentType(cTypeDesc);
				unsigned bufferViewNdx = accessorDesc["bufferView"];
				unsigned byteOffset = 0;
				auto offsetIter = accessorDesc.find("byteOffset");
				if(offsetIter != accessorDesc.end())
					byteOffset = offsetIter.value().get<unsigned>();
				accessor.count = accessorDesc["count"];
				accessor.data = &bufferViews[bufferViewNdx].data[byteOffset];
				accessors.push_back(accessor);
			}

			// Return the right scene
			auto& displayScene = scenesDict.value()[(unsigned)scene.value()];
			return nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		core::Time::init();

		assert(!mGfxDriver);
		mGfxDriver = GraphicsDriverGL::createDriver(_window);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		if(mGfxDriver) {
			//loadScene("sponza_crytek");
			loadGLTFScene("helmet/");
			createCamera();

			mGameEditor.init(mGraphicsScene);
			mRenderer.init(*mGfxDriver, *mGfxDriver->frameBuffer());
			gui::init(_window->size);
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {
		// Node
		auto cameraNode = std::make_shared<SceneNode>();
		mGameScene.root()->addChild(cameraNode);
		cameraNode->name = "Camera";
		// Transform
		auto objXForm = std::make_unique<Transform>();
		objXForm->matrix().setIdentity();
		objXForm->xForm.position() = math::Vec3f { 400.f, 120.f, 170.f };
		objXForm->xForm.setRotation(math::Quatf(Vec3f(0.f,0.f,1.f), 1.57f));
		cameraNode->addComponent(std::move(objXForm));
#ifdef _WIN32
#endif
		// Actual camera
		auto camComponent = std::make_unique<game::Camera>();
		mCamera = &camComponent->cam();
		cameraNode->addComponent(std::move(camComponent));
		// Init camera
		cameraNode->init();
	}

	struct MeshHeader
	{
		uint32_t nVertices;
		uint32_t nIndices;
	};

	struct RenderObjData {
		int meshIdx = -1;
		Mat44f transform;
	};

	namespace {
		struct MeshFactory
		{
			MeshFactory(
				const game::ModelAsset& meshPool,
				graphics::RenderScene& scene
			)
				: mMeshPool(meshPool)
				, mScene(scene)
			{}

			// TODO: Also use materials from the material manager, or some alternative way of loading materials
			// Factory signature: std::unique_ptr<Component>(const std::string&, std::istream&)
			std::unique_ptr<game::Component> operator()(const std::string&, std::istream& in) const
			{
				std::string sceneName, materialName;
				in >> sceneName;
				in.get();
				uint32_t nMeshes;
				in.read((char*)&nMeshes, sizeof(nMeshes));
				std::vector<uint32_t>  meshList(nMeshes);
				auto renderObj = std::make_shared<RenderObj>();
				for(auto& meshIdx : meshList)
				{
					in.read((char*)&meshIdx, sizeof(uint32_t));
					renderObj->meshes.push_back(mMeshPool.mesh(meshIdx));
					// TODO: Actually load materials
					in >> materialName;
					in.get();
					renderObj->materials.push_back(nullptr);
				}
				mScene.renderables().push_back(renderObj);
				return std::make_unique<MeshRenderer>(
					renderObj,
					meshList,
					sceneName);
			}

		private:
			const game::ModelAsset& mMeshPool;
			graphics::RenderScene& mScene;
		};
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const std::string& _assetFileName)
	{
		// TODO: Use a real geometry pool. Even better. Use a geometry pool and a model mananger to load geometry.
		// Even better: Do that in a background thread while other components are loaded
		game::ModelAsset geometryPool(_assetFileName + ".mdl");
		// Load the scene components
		core::File asset(_assetFileName + ".scn");
		if(asset.sizeInBytes() == 0)
		{
			rev::core::Log::error("Unable to load asset");
			return;
		}
		auto& in = asset.asStream();
		mComponentFactory.registerFactory(
			"MeshRenderer",
			MeshFactory(geometryPool, mGraphicsScene),
			true);
		// Load scene nodes
		if(!mGameScene.load(in, mComponentFactory))
		{
			rev::core::Log::error("Error loading scene");
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		if(!mGfxDriver)
			return true;
		core::Time::get()->update();
		gui::startFrame();
		mGameEditor.update(mGameScene);

		auto dt = core::Time::get()->frameTime();

		mGameScene.root()->update(dt);

		mRenderer.render(*mCamera, mGraphicsScene);

		gui::finishFrame(dt);
		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev