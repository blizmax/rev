//----------------------------------------------------------------------------------------------------------------------
// Rev SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// On 2013/01/15
//----------------------------------------------------------------------------------------------------------------------
// Simple tool to test shaders

#include <revPlatform/fileSystem/fileSystem.h>
#include <revPlatform/osHandler/osHandler.h>
#include <revVideo/videoDriver/videoDriver.h>
#include <revVideo/driver3d/openGL21/driver3dOpenGL21.h>

using namespace rev;
using namespace rev::video;

int main(int , const char**)
{
	VideoDriver::startUp();
	VideoDriver * vDriver = VideoDriver::get();
	// OpenGL specific code
	// Override default initialization to use a custom shader
	vDriver->init3d();
	Driver3dOpenGL21* driver3d = static_cast<Driver3dOpenGL21*>(VideoDriver::getDriver3d());
	// const char testVtx[] = "test.vtx";
	// const char testPxl[] = "test.pxl";
	// unsigned shader = driver3d->loadShader("test.vtx","test.pxl");
	// driver3d->setShader(shader);
	// 
	// // Shader auto-reload
	// auto shaderReload = [&](const char*){
	// 	driver3d->glDeleteProgram(shader);
	// 	shader = driver3d->loadShader(testVtx, testPxl);
	// 	driver3d->setShader(shader);
	// 	};
	// 
	// platform::FileSystem::get()->onFileChanged(testVtx) += shaderReload;
	// platform::FileSystem::get()->onFileChanged(testPxl) += shaderReload;

	bool mustExit = false;
	while(!mustExit) {
		driver3d->finishFrame();
		mustExit = !vDriver->update();
		mustExit |= !platform::OSHandler::get()->update();
	}
	
	return 0;
}