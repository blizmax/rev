////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input

#include "keyboardInput.h"
#ifdef WIN32
#include "windows/keyboardInputWindows.h"
#endif // WIN32

#include <cassert>

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	KeyboardInput * KeyboardInput::sInstance = 0;

	//------------------------------------------------------------------------------------------------------------------
	void KeyboardInput::init()
	{
		assert(0 == sInstance);
		sInstance = new KeyboardInput();
		assert(0 != sInstance);
	}


	//------------------------------------------------------------------------------------------------------------------
	void KeyboardInput::end()
	{
		assert(0 != sInstance);
		delete sInstance;
		sInstance = 0;
	}
}	// namespace input
}	// namespace rev