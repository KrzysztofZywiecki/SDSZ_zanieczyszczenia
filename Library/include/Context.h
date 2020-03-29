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
        static void InitVulkan(GLFWwindow* window);
        static void CleanUP();

        static VkDevice device;

        private:
        static VkDebugUtilsMessengerEXT debugMessenger;
        static VkInstance instance;
        static VkPhysicalDevice physicalDevice;
        static VkQueue commandQueue;
        static VkSurfaceKHR surface;
        
        static uint32_t queueFamilyIndex;


        static std::vector<const char*> GetRequiredExtensions();
        static bool CheckValidationSupport();

        static void CreateInstance();
        static void SetupDebugMessenger();
        static void CreateSurface(GLFWwindow* window);
        static void PickPhysicalDevice();
        static void CreateDevice();
    };

}