//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_

#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/shader/shader.h>

namespace rev {
	namespace video {

		class Camera;
		class RendererBackEnd;
		struct RenderScene;
		class RenderTarget;

		class ForwardRenderer {
		public:
			void init(GraphicsDriver* _driver);
			// TODO: Instead of a camera, this should receive a "render info" struct with
			// information like: use shadows? LOD bias? cam, etc
			// nullptr render target means: use framebuffer
			void render(const RenderScene& _scene, const Camera& _cam, const RenderTarget* _rt = nullptr) const;

		private:
			GraphicsDriver*	mDriver;
			Shader::Ptr	mProgram;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_