////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on 2014-10-29
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input for linux

#ifdef __linux__
#ifndef _REV_INPUT_KEYBOARD_LINUX_KEYBOARDINPUTLINUX_H_
#define _REV_INPUT_KEYBOARD_LINUX_KEYBOARDINPUTLINUX_H_

namespace rev { namespace input
{
	class KeyboardInputLinux
	{
	public:
		enum EKeyCode
		{
			eBackSpace,
			eTab,
			eEnter,
			eShift,
			eControl,
			eAlt,
			ePause,
			eCapsLock,
			eEscape,
			eSpace,
			ePageUp,
			ePageDown,
			eEnd,
			eHome,
			eKeyLeft,
			eKeyUp,
			eKeyRight,
			eKeyDown,
			eA,
			eB, eC, eD, eE, eF, eG, eH, eI, eJ, eK, eL, eM, eN, eO, eP, eQ, eR, eS, eT, eU, eV, eW, eX, eY, eZ,

			eMaxKeyCode
		};
	public:
		KeyboardInputLinux() {}
		~KeyboardInputLinux() {}

		bool pressed	(EKeyCode _key) const { return false; }
		bool held		(EKeyCode _key) const { return false; }
		bool released	(EKeyCode _key) const { return false; } 
		void refresh	() {}

	private:
		// int keyState[eMaxKeyCode];
		// int oldKeyState[eMaxKeyCode];
	};

	typedef KeyboardInputLinux	KeyboardInputBase;

}	// namespace input
}	// namespace rev

#endif // _REV_INPUT_KEYBOARD_LINUX_KEYBOARDINPUTLINUX_H_
#endif // __linux__