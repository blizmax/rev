//----------------------------------------------------------------------------------------------------------------------
// Rev Generic template
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On October 23rd, 2012
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <iostream>
#include <revCore/codeTools/log/log.h>
#include <revVideo/videoDriver/videoDriver.h>

int main (int //_argc
	, const char** //_argv
	)
{
	rev::codeTools::Log::init();
	rev::video::VideoDriver::startUp();
	rev::video::VideoDriver::get()->createMainWindow();

	bool mustExit = false;
	while(!mustExit)
	{
		mustExit = !rev::video::VideoDriver::get()->update();
	}
	
	rev::video::VideoDriver::shutDown();
	rev::codeTools::Log::end();
	return 0;
}