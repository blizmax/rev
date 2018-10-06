//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef _WIN32

#include <graphics/backend/OpenGL/openGL.h>
#include <math/algebra/vector.h>
#include <graphics/driver/DefaultFrameBuffer.h>

namespace rev::gfx {

	class WindowWin32
	{
	public:
		HWND nativeWindow;
		math::Vec2u size;

		static WindowWin32 createWindow(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName, bool visible = true);
	};

	class GraphicsDriverGL;

	class GraphicsDriverGLWindows
	{
	public:
		using NativeWindow = WindowWin32*;

		void onWindowResize(const math::Vec2u& _size)
		{
			nativeWindow()->size = _size;
			frameBuffer()->resize(_size);
		}

		virtual DefaultFrameBuffer* frameBuffer() const = 0;

		static GraphicsDriverGL* createDriver(NativeWindow _nativeWindow);
		NativeWindow nativeWindow() const { return window; }
		void swapBuffers();

	protected:
		bool mSupportSRGBFrameBuffer = false;
		bool mSupportSRGBTextures = false;

	private:
		static void checkExtensions(bool& sRGBTextures, bool& sRGBFrameBuffer);

		NativeWindow	window;
		HWND	mWindowHandle;
		HDC		mDevCtxHandle;
	};

	typedef GraphicsDriverGLWindows GraphicsDriverGLBase;

} // namespace rev::gfx

#endif // _WIN32