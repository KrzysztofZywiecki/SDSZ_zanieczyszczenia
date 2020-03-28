#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <iostream>

namespace Library
{

    class Context
    {
        public:
        static void InitVulkan();
        static void CleanUP();

        private:
        static VkInstance instance;
        static VkDevice device;
        static VkDebugUtilsMessengerEXT debugMessenger;
        

        static std::vector<const char*> GetRequiredExtensions();
        static bool CheckValidationSupport();

        static void CreateInstance();
        static void SetupDebugMessenger();
        static void PickPhysicalDevice();
        static void CreateDevice();


    };

}