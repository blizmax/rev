//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef _WIN32

#include "../openGL.h"
#include <math/algebra/vector.h>

namespace rev { namespace graphics {

	class WindowWin32
	{
	public:
		HWND nativeWindow;
		math::Vec2u size;

		//
	};

	class GraphicsDriverGL;

	class GraphicsDriverGLWindows
	{
	public:
		using NativeWindow = WindowWin32*;

		static GraphicsDriverGL* createDriver(NativeWindow _nativeWindow);
		void swapBuffers();

	private:
		HWND	mWindowHandle;
		HDC		mDevCtxHandle;
	};

	typedef GraphicsDriverGLWindows GraphicsDriverGLBase;

}} // namespace rev::graphics

#endif // _WIN32