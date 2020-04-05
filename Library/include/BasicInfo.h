#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Window.h"
#include <iostream>
#include <stdexcept>
#include <vector>

#ifndef EXT_LISTS
#define EXT_LISTS
    #ifdef NDEBUG
        bool validationEnabled = false;
    #else
        bool validationEnabled = true;
    #endif

    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif

namespace Library
{
    class Instance
    {
        public:
            Instance();

            void Destroy();
            void Create(Window& window);
            VkInstance GetInstance();
            VkSurfaceKHR GetSurface();
        private:
            void CreateInstance();
            void CreateSurface(Window& window);
            void CreateDebugCallback();

            VkInstance instance;
            VkSurfaceKHR surface;
            VkDebugUtilsMessengerEXT debugMessenger;
    };


}

