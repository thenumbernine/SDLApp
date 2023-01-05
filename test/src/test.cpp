#include "SDLApp/SDLApp.h"
#include "Common/Exception.h"
#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

struct Test : public ::SDLApp::SDLApp {
	using Super = ::SDLApp::SDLApp;

protected:
	virtual void initWindow() {
		Super::initWindow();

		//https://www.gamedev.net/forums/topic/699117-vulkan-with-sdl2-getting-started/

		uint32_t extensionCount;
		SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
		std::vector<const char *> extensionNames(extensionCount);
		SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames.data());

		VkApplicationInfo appInfo {};
		// TODO: fill this out

		std::vector<const char *> layerNames;
		// uncomment below if you want to use validation layers
		// layerNames.push_back("VK_LAYER_LUNARG_standard_validation");

		VkInstanceCreateInfo info {};
		info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		info.pApplicationInfo = &appInfo;
		info.enabledLayerCount = layerNames.size();
		info.ppEnabledLayerNames = layerNames.data();
		info.enabledExtensionCount = extensionNames.size();
		info.ppEnabledExtensionNames = extensionNames.data();

		VkResult res;
		VkInstance instance;
		res = vkCreateInstance(&info, nullptr, &instance);
		if (res != VK_SUCCESS) {
			throw Common::Exception() << "vkCreateInstance failed: " << res;
		}


		// now that you have a window and a vulkan instance you need a surface
		VkSurfaceKHR surface;
		if (!SDL_Vulkan_CreateSurface(window, instance, &surface)) {
			throw Common::Exception() << "SDL_Vulkan_CreateSurface failed";
		}

		// at this point you have a window, vulkan instance and a vulkan surface
		// SDL is out of your way and you can use the vulkan api to enumerate the
		// physical devices, create a logical device plus queues, and create your
		// swapchain.
	}
	
	std::string getTitle() {
		return "Vulkan Test";
	}
	
	virtual Uint32 getSDLCreateWindowFlags() {
		return Super::getSDLCreateWindowFlags() | SDL_WINDOW_VULKAN;
	}

};

SDLAPP_MAIN(Test)
