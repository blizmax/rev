////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on October 18th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Touch input

#ifndef _REV_REVINPUT_TOUCHINPUT_TOUCHINPUT_H_
#define _REV_REVINPUT_TOUCHINPUT_TOUCHINPUT_H_

namespace rev { namespace input
{
	class STouchInput
	{
	public:
		// Singleton interface
		static	void			init	();
		static	void			deinit	();
		static	STouchInput*	get		();

		// Public interface for touch input
		virtual unsigned		nActiveTouches	() const = 0;	// The number of points currently "touching" the screen

		virtual bool			pressed		(unsigned _touch = 0) const = 0; // The given touch just pressed?
		virtual bool			held		(unsigned _touch = 0) const = 0; // Is the given touch held?
		virtual bool			released	(unsigned _touch = 0) const = 0; // Has the given touch just been released?

		virtual	TVec2			touchPos	(unsigned _touch = 0) const = 0; // Current position of the given touch
	protected:
		STouchInput()			{}
		virtual ~STouchInput()	{}

		static STouchInput * sTouchInput;
	};

}	// namespace input
}	// namespace rev

#endif // _REV_REVINPUT_TOUCHINPUT_TOUCHINPUT_H_
