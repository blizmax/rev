//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vulkan/vulkan.h>

namespace rev {
	namespace video {

		class Window;

		class VulkanDriver {
		public:
#ifdef ANDROID
			VulkanDriver();
#else
			VulkanDriver(Window*);
#endif
			~VulkanDriver();

		private:
			void createInstance();
			void queryExtensions(VkInstanceCreateInfo&);
			void getPhysicalDevice();

			VkInstance					mApiInstance;
			VkPhysicalDevice			mDevice = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties	mDeviceProps;
			VkPhysicalDeviceFeatures	mDeviceFeatures;
			VkExtensionProperties*		mExtensions = nullptr;
		};
} }
