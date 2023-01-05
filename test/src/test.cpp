#include "SDLApp/SDLApp.h"
#include "Common/Exception.h"
#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <iostream>	//debugging only
#include <set>

//https://www.gamedev.net/forums/topic/699117-vulkan-with-sdl2-getting-started/
//https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance

struct Test : public ::SDLApp::SDLApp {
	using Super = ::SDLApp::SDLApp;

protected:
#if 0
	static constexpr bool const useValidationLayers = true;
#else
	static constexpr bool const useValidationLayers = false;
#endif
	
	virtual void initWindow() {
		Super::initWindow();
		initVulkan();
	}

	std::string getTitle() {
		return "Vulkan Test";
	}
	
	virtual Uint32 getSDLCreateWindowFlags() {
		return Super::getSDLCreateWindowFlags() | SDL_WINDOW_VULKAN;
	}


	virtual void initVulkan() {
		initVulkanInstance();
		initVulkanSurface();
		initVulkanPhysicalDevice();
		initVulkanLogicalDevice();
	}

	VkInstance vkinstance = {};
	
	virtual void initVulkanInstance() {
		{
			std::vector<VkLayerProperties> availableLayers;
			
			uint32_t layerCount = {};
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			availableLayers.resize(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		
			std::cout << "vulkan layers:" << std::endl;
			for (auto const & layer : availableLayers) {
				std::cout << layer.layerName << std::endl;
			}
		}

		std::vector<const char *> extensionNames;
		{
			uint32_t extensionCount = {};
			SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
			extensionNames.resize(extensionCount);
			SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames.data());

			std::cout << "vulkan extensions:" << std::endl;
			for (auto const & ext : extensionNames) {
				std::cout << ext << std::endl;
			}
		}

		VkApplicationInfo appInfo {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		std::vector<const char *> layerNames;
		if (useValidationLayers) {
			//layerNames.push_back("VK_LAYER_LUNARG_standard_validation");	//nope
			layerNames.push_back("VK_LAYER_KHRONOS_validation");	//nope
		}

		VkInstanceCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = layerNames.size();
		createInfo.ppEnabledLayerNames = layerNames.data();
		createInfo.enabledExtensionCount = extensionNames.size();
		createInfo.ppEnabledExtensionNames = extensionNames.data();

		{
			VkResult res = vkCreateInstance(&createInfo, nullptr, &vkinstance);
			if (res != VK_SUCCESS) {
				throw Common::Exception() << "vkCreateInstance failed: " << res;
			}
		}

		// now that you have a window and a vulkan instance you need a surface
		VkSurfaceKHR surface;
		if (!SDL_Vulkan_CreateSurface(window, vkinstance, &surface)) {
			throw Common::Exception() << "SDL_Vulkan_CreateSurface failed";
		}

		// at this point you have a window, vulkan instance and a vulkan surface
		// SDL is out of your way and you can use the vulkan api to enumerate the
		// physical devices, create a logical device plus queues, and create your
		// swapchain.
	}
	
	VkSurfaceKHR vksurface;
		
	virtual void initVulkanSurface() {
		// TODO YOU ARE HERE
		// https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface

		if (!SDL_Vulkan_CreateSurface(window, vkinstance, &vksurface)) {
			 throw Common::Exception() << "vkCreateWaylandSurfaceKHR failed";
		}
	}

	VkPhysicalDevice physicalDevice = {};
		
	virtual void initVulkanPhysicalDevice() {
		uint32_t deviceCount = {};
		vkEnumeratePhysicalDevices(vkinstance, &deviceCount, nullptr);
		if (!deviceCount) {
			throw Common::Exception() << "failed to find GPUs with Vulkan support!";
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vkinstance, &deviceCount, devices.data());
		
		std::cout << "devices:" << std::endl;
		for (auto const & device : devices) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			std::cout << deviceProperties.deviceName 
				<< " type=" << deviceProperties.deviceType
				<< std::endl;
		}

		auto i = std::find_if(devices.begin(), devices.end(), [this](VkPhysicalDevice d) -> bool {
			return this->pickDevice(d);
		});
		if (i == devices.end()) {
			throw Common::Exception() << "failed to find a suitable GPU!";
		}
		physicalDevice = *i;
	}

	//override callback for picking device.
	// TODO like CLCommon.  get from list, override sorting function or something. idk.
	virtual bool pickDevice(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		// TODO sort by score and pick the best
		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
			|| deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
			|| deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
		;
			// && deviceFeatures.geometryShader;
	}

	VkDevice vkdevice = {};
	VkQueue graphicsQueue;
	VkQueue presentQueue;
		
	virtual void initVulkanLogicalDevice() {
		// tut has this in the end of initVulkanPhysicalDevice 
		// probably so it can inquire each device to see if its queuefamilies has the right bits.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		//tut uses optional<uint32_t> instead of -1's
		uint32_t graphicsFamilyIndex = -1u;
		uint32_t presentFamilyIndex = -1u;
		for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
			auto f = queueFamilies[i];
			if (f.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphicsFamilyIndex = i;
			}
			
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vksurface, &presentSupport);
			if (presentSupport) {
				presentFamilyIndex = i;
			}
		}
		if (graphicsFamilyIndex == -1u) {
			throw Common::Exception() << "couldn't find a VkQueueFamily with VK_QUEUE_GRAPHICS_BIT";
		}
		if (presentFamilyIndex == -1u) {
			throw Common::Exception() << "couldn't find a VkQueueFamily with VK_QUEUE_GRAPHICS_BIT";
		}


		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		{
			std::set<uint32_t> uniqueQueueFamilies = {graphicsFamilyIndex, presentFamilyIndex};
			float queuePriority = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies) {
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}
		}


		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		//VkPhysicalDeviceFeatures deviceFeatures = {}; // empty
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.enabledExtensionCount = 0;
		if (useValidationLayers) {
		// TODO remind me again where validationLayers is from?
		//    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		//    createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}
		{
			VkResult res = vkCreateDevice(physicalDevice, &createInfo, nullptr, &vkdevice);
			if (res != VK_SUCCESS) throw Common::Exception() << "vkCreateDevice failed: " << res;
		}
	
		vkGetDeviceQueue(vkdevice, graphicsFamilyIndex, 0, &graphicsQueue);
		vkGetDeviceQueue(vkdevice, presentFamilyIndex, 0, &presentQueue);
	}

public:
	~Test() {
		if (vksurface) vkDestroySurfaceKHR(vkinstance, vksurface, nullptr);
		if (vkdevice) vkDestroyDevice(vkdevice, nullptr);
		if (vkinstance) vkDestroyInstance(vkinstance, nullptr);
	}
};

SDLAPP_MAIN(Test)
