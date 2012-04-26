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

#include <revCore/codeTools/assert/assert.h>

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	SKeyboardInput * SKeyboardInput::sInstance = 0;

	//------------------------------------------------------------------------------------------------------------------
	void SKeyboardInput::init()
	{
		revAssert(0 == sInstance);
#ifdef WIN32
		sInstance = new CKeyboardInputWindows();
#endif // WIN32
		revAssert(0 != sInstance);
	}

	
	//------------------------------------------------------------------------------------------------------------------
	void SKeyboardInput::end()
	{
		revAssert(0 != sInstance);
		delete sInstance;
		sInstance = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	SKeyboardInput * SKeyboardInput::get()
	{
		return sInstance;
	}
}	// namespace input
}	// namespace rev