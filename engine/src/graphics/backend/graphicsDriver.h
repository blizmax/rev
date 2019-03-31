//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include "commandQueue.h"

namespace rev::gfx
{
	class Device;

	/// Interface for dealing with low level tasks of graphics APIs.
	class GraphicsDriver
	{
	public:
		struct PhysicalDeviceInfo
		{
			float shaderModel = 2;
			size_t dedicatedVideoMemory = 0;

			int deviceApiIndex; // Used to actually create the physical device with the underlying api.
		};

		struct PhysicalDevice {};

		/// \return the number of devices found
		virtual int enumeratePhysicalDevices(PhysicalDeviceInfo* dst, int maxDevices) = 0;

		// -- Device management --
		virtual PhysicalDevice* createPhysicalDevice(int deviceIndex) = 0;

		/// Create a new graphics device hosted in \param adapter and containing the requested command queues
		virtual Device* createDevice(const PhysicalDevice& adapter, int numQueues, const CommandQueue::Info* commandQueueDesc) = 0;
	};
}
