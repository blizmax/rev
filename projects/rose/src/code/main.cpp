//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <codeTools/log/log.h>
#include <iostream>

#include <component/microcontroller/avr/avr8bit.h>

using namespace rev;

using rose::component::Avr8bit;

int main()
{
	// System initialization
	codeTools::Log::init();
	revLog() << "Rose: Robotic Simulation Environment\n";

	// Actual code
	revLog() << "Creating AVR Core ... ";
	Avr8bit * core = new Avr8bit(256, 8);
	revLog() << "Success\n";
	core->loadProgram("ciervaPilot.hex");
	revLog() << "Run 10 cycles\n";
	core->simulate(10);
	core->reset();
	core->showAssembly(110, 120);
	core->showMemory(0x21f0, 0x2200);


	// House keeping
	codeTools::Log::end();
	return 0;
}