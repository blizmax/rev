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

namespace rev {
	namespace video {

		class GraphicsDriver;
		class RenderContext;
		class Window;
		class RenderObj;

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
			//void renderDebug(const RenderObj&);

		private:
			void renderObject(const RenderObj& _obj);

		private:
			Shader::Ptr			mShader = nullptr;
			RendererBackEnd*	mBackEnd;
			rev::math::Vec2u	mWindowSize;
			math::Vec3f			mLightPos, mLightDir;
			GraphicsDriver*		mDriver;
			ShadowPass	*		mShadowPass;
		};

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::init(GraphicsDriver* _driver, Alloc_& _alloc){
			mDriver = _driver;
			mBackEnd = _alloc.template create<RendererBackEnd>(mDriver);
			mShader = Shader::manager()->get("forward");

			mShadowPass = new ShadowPass(mDriver, mBackEnd, 2048);

			mDriver->setShader((Shader*)mShader);
			mDriver->setClearColor(Color(0.7f, 0.8f, 1.f, 1.f));

			mLightPos = {0.0, 0.0, 10.0};
			mLightDir = math::Vec3f(1.f, 0.f, 0.f).normalized();
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::end(Alloc_& _alloc){
			delete mShadowPass;
			_alloc.destroy(mBackEnd);
		}
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_