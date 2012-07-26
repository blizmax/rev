//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Log system

#include "log.h"

namespace rev { namespace codeTools
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data
	Log* Log::sInstance = nullptr;

	//------------------------------------------------------------------------------------------------------------------
	void Log::init()
	{
		sInstance = new Log();
	}
}	// namespace codeTools
}	// namespace rev