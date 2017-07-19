//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifdef REV_USE_VULKAN
#include "vulkanDriver.h"

#define VK_USE_PLATFORM_ANDROID_KHR 1
#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

#include <video/window/window.h>
#include <iostream>
#include <string>
#include <vector>

#ifdef ANDROID
#include <android/native_window.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "rev.AndroidPlayer", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "rev.AndroidPlayer", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "rev.AndroidPlayer", __VA_ARGS__))

#endif // ANDROID

using namespace std;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
#ifdef ANDROID
		VulkanDriver::VulkanDriver() {
#else
		VulkanDriver::VulkanDriver(Window* _wnd) {
#endif
			createInstance();
#if _DEBUG
			setupDebugCallback();
#endif
#ifdef _WIN32
			if(!initSurface(_wnd))
				return;
#else
			if(!initSurface())
				return;
#endif
			getPhysicalDevice();
			findQueueFamilies();
			createLogicalDevice();
		}

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::~VulkanDriver() {
			vkDestroyDevice(mDevice, nullptr);
			delete[] mExtensions;
			vkDestroySurfaceKHR(mApiInstance, mSurface, nullptr);
			vkDestroyInstance(mApiInstance, nullptr);
		}

		namespace
		{
			//----------------------------------------------------------------------------------------------------------
			const std::vector<const char*> validationLayers = {
				"VK_LAYER_LUNARG_standard_validation"
			};

			//----------------------------------------------------------------------------------------------------------
			std::vector<const char*> getRequiredExtensions() {
				std::vector<const char*> extensions;
#ifdef _DEBUG
					extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

				return extensions;
			}

			//----------------------------------------------------------------------------------------------------------
			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
				VkDebugReportFlagsEXT flags,
				VkDebugReportObjectTypeEXT objType,
				uint64_t obj,
				size_t location,
				int32_t code,
				const char* layerPrefix,
				const char* msg,
				void* userData) {

				std::cout << "validation layer: " << msg << std::endl;

				return VK_FALSE;
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createInstance() {
			if(!checkValidationLayerSupport())
				return;

			// Basic application info
			VkApplicationInfo appInfo = {};
			appInfo.apiVersion = VK_API_VERSION_1_0;
			appInfo.pEngineName = "RevEngine";
			appInfo.pApplicationName = "RevPlayer";
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			// Instance creation info
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
#ifdef _DEBUG
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
#else
			createInfo.enabledLayerCount = 0;
#endif
			auto extensions = getRequiredExtensions();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			// Query extensions
			queryExtensions(createInfo);

			VkResult res = vkCreateInstance(&createInfo, nullptr, &mApiInstance);
			if (res != VK_SUCCESS)
			{
				cout << "Error: Unable to create vulkan instance.\n";
				//LOGE("Error: Unable to create vulkan instance.\n");
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		bool VulkanDriver::checkValidationLayerSupport() {
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
			for (const char* layerName : validationLayers) {
				bool layerFound = false;

				for (const auto& layerProperties : availableLayers) {
					if (strcmp(layerName, layerProperties.layerName) == 0) {
						layerFound = true;
						break;
					}
				}

				if (!layerFound) {
					assert(false);
					return false;
				}
			}

			return true;
		}

		//----------------------------------------------------------------------------------------------------------
		void VulkanDriver::setupDebugCallback() {
			VkDebugReportCallbackCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
			createInfo.pfnCallback = debugCallback;

			auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(mApiInstance, 
																"vkCreateDebugReportCallbackEXT");
			if ((func == nullptr)
			  ||(func(mApiInstance, &createInfo, nullptr, &mDebugCallback) != VK_SUCCESS))
			{
				cout << "Warning: Unable to set Vulkan debug callback.\n"
					<< "You won't be receiving notifications from validation layers\n";
				return;
			}
		}

		//--------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
		bool VulkanDriver::initSurface(Window* _wnd) {
			VkWin32SurfaceCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.hwnd = _wnd->winapiHandle();
			createInfo.hinstance = GetModuleHandle(nullptr);

			// Get the extension
			auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(mApiInstance, "vkCreateWin32SurfaceKHR");

			// Create the surface
			if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(mApiInstance, &createInfo, nullptr, &mSurface) != VK_SUCCESS) {
				cout << "failed to create window surface!\n";
				return false;
			}

			return true;
		}
#endif // _WIN32
#ifdef ANDROID
		bool VulkanDriver::initSurface() {
			LOGI("Init surface");
			// Get display information
			//uint32_t displayPropCount = 0;
			//vkGetPhysicalDeviceDisplayPropertiesKHR(mPhysicalDevice, &displayPropCount, nullptr);

			// Get extension
			auto fpCreateAndroidSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)vkGetInstanceProcAddr(mApiInstance, "vkCreateAndroidSurfaceKHR");
			if (fpCreateAndroidSurfaceKHR == nullptr) {
				//LOGE("Unable to get create surface extension");
				return false;
			}

			// Set display mode
			/*VkDisplayModeKHR

			VkDisplaySurfaceCreateInfoKHR	createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
			createInfo.*/
			LOGI("Finish Init surface");
			return true;
		}
#endif // ANDROID

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::queryExtensions(VkInstanceCreateInfo& _ci) {
			// Query available extensions count
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, nullptr);
			//LOGI("Found %d vulkan extensions:\n" , _ci.enabledExtensionCount);

			// Allocate space for extension names
			mExtensions = new VkExtensionProperties[_ci.enabledExtensionCount];
			char** extensionNames = new char*[_ci.enabledExtensionCount];
			_ci.ppEnabledExtensionNames = extensionNames;
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, mExtensions);

			// Copy extension names into createInfo
			for (size_t i = 0; i < _ci.enabledExtensionCount; ++i) {
				extensionNames[i] = mExtensions[i].extensionName;
#if _DEBUG
				cout << "- " << mExtensions[i].extensionName << "\n";
#endif
				//LOGI((std::string("- ") + mExtensions[i].extensionName).c_str());
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::getPhysicalDevice() {
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(mApiInstance, &deviceCount, nullptr);
			//LOGI("Vulkan: Found  %d physical devices\n", deviceCount);

			VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
			vkEnumeratePhysicalDevices(mApiInstance, &deviceCount, devices);	
			mPhysicalDevice = devices[0];
			delete[] devices;

			// Get device properties
			vkGetPhysicalDeviceProperties(mPhysicalDevice, &mDeviceProps);
			cout << "Vulkan device name: " << mDeviceProps.deviceName << "\n";
			//LOGI("Vulkan device name: %s", mDeviceProps.deviceName);
			vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mDeviceFeatures);
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::findQueueFamilies() {
			uint32_t familyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount, nullptr);

			std::vector<VkQueueFamilyProperties>	families(familyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount, families.data());

			int index = 0;
			for (const auto& family : families) {
				if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT && family.queueCount > 0)
					mQueueFamilyIndex = index;

				++index;
			}
			
			assert(mQueueFamilyIndex >= 0);
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createLogicalDevice() {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = mQueueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			float queuePriority = 1.0f;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pQueueCreateInfos = &queueCreateInfo;
			createInfo.queueCreateInfoCount = 1;
			createInfo.pEnabledFeatures = &mDeviceFeatures;

			createInfo.enabledExtensionCount = 0;
#if _DEBUG
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
#else
			createInfo.enabledLayerCount = 0;
#endif

			if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
				 //LOGE("failed to create logical device!");
			}

			vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 0, &mGraphicsQueue);
		}
	}
}

#endif // REV_USE_VULKAN