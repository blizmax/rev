//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_

#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <video/basicTypes/color.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/driver/openGL/openGLDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>
#include <video/window/window.h>
#include <video/graphics/renderer/renderPass/shadowPass.h>
#include <video/graphics/renderer/debugDrawer.h>

namespace rev {
	namespace video {

		class GraphicsDriver;
		class RenderContext;
		class Window;
		class RenderObj;
		class Camera;

		class ForwardRenderer {
		public:
			template<class Alloc_> void init(GraphicsDriver* _driver, Alloc_& _alloc);
			template<class Alloc_> void end(Alloc_& _alloc);

			void startFrame();
			void finishFrame();

			// Renderer's specific config
			void setShadowLight(const math::Vec3f& _pos, const math::Vec3f& dir, const Color& _color);
			void setAmbientLight(const Color& _color);
			void setWindowSize(const rev::math::Vec2u& _size) { mWindowSize = _size; }

			void renderContext(const RenderContext&);
			void setDebugCamera(const Camera* _dbg) { mDebugCamera = _dbg; }

		private:
			void renderObject(const RenderObj& _obj);

		private:
			DebugDrawer*		mDebug;
			const Camera*		mDebugCamera;
			Shader::Ptr			mShader = nullptr;
			RendererBackEnd*	mBackEnd;
			rev::math::Vec2u	mWindowSize;
			math::Vec3f			mLightPos, mLightDir;
			GraphicsDriver*		mDriver;
			ShadowPass	*		mShadowPass;
			ShadowPass*			mFarShadowPass;
		};

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::init(GraphicsDriver* _driver, Alloc_& _alloc){
			mDriver = _driver;
			mBackEnd = _alloc.template create<RendererBackEnd>(mDriver);
			mShader = Shader::manager()->get("forward");
			mDebug = new DebugDrawer(mDriver, mBackEnd);

			mShadowPass = new ShadowPass(mDriver, mBackEnd, 1024);
			mFarShadowPass = new ShadowPass(mDriver, mBackEnd, 1024);

			mDriver->setShader((Shader*)mShader);

			mLightPos = {0.0, 0.0, 10.0};
			mLightDir = math::Vec3f(1.f, 0.f, -1.f).normalized();
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::end(Alloc_& _alloc){
			delete mShadowPass;
			delete mFarShadowPass;
			_alloc.destroy(mBackEnd);
		}
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_