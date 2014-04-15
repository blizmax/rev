//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_

#include <video/basicTypes/color.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/driver/openGL/openGLDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>
#include <video/window/window.h>

namespace rev {
	namespace video {

		class GraphicsDriver;
		class RenderObj;
		class Window;

		class ForwardRenderer {
		public:
			template<class Alloc_> void init(Window* _window, Alloc_& _alloc);
			template<class Alloc_> void end(Alloc_& _alloc);

			void startFrame();
			void finishFrame();
			void renderObject(const RenderObj&);

		private:
			video::Shader::Ptr		mShader = nullptr;
			video::RendererBackEnd*	mBackEnd;
			GraphicsDriver*			mDriver;
		};

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::init(Window* _window, Alloc_& _alloc){
			mDriver = _alloc.create<OpenGLDriver>(_window);
			mBackEnd = _alloc.create<RendererBackEnd>(mDriver);
			mShader = Shader::manager()->get("test");

			mDriver->setShader(mShader);
			mDriver->setClearColor(Color(0.7f, 0.8f, 1.f, 1.f));
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Alloc_> void ForwardRenderer::end(Alloc_& _alloc){
			_alloc.destroy(mBackEnd);
			_alloc.destroy(mDriver);
		}
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_