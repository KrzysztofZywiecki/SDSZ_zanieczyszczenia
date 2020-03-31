#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Window.h"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>

namespace Library
{

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class Context
    {
        public:
        static void InitVulkan(Window* window);
        static void CleanUP();

        static VkDevice device;

        private:
        static VkDebugUtilsMessengerEXT debugMessenger;
        static VkInstance instance;
        static VkSurfaceKHR surface;
        static SwapChainSupportDetails capabilities;
        static VkPhysicalDevice physicalDevice;
        static VkQueue commandQueue;
        static VkSwapchainKHR swapChain;
        static std::vector<VkImage> swapChainImages;
        static std::vector<VkImageView> swapChainImageViews;
        static VkPipeline graphicsPipeline;
        static VkPipelineLayout graphicsPipelineLayout;
        static VkRenderPass renderPass;

        static uint32_t queueFamilyIndex;
        static VkFormat swapChainImageFormat;
        static VkExtent2D windowExtent;

        static std::vector<const char*> GetRequiredExtensions();
        static bool CheckValidationSupport();
        static VkShaderModule CreateShaderModule(const char* path);

        static void CreateInstance();
        static void SetupDebugMessenger();
        static void CreateSurface(GLFWwindow* window);
        static void PickPhysicalDevice();
        static void CreateDevice();
        static void CreateSwapChain(Window* window);
        static void CreateSwapChainImageViews();
        static void CreatePipelineLayout();
        static void CreateRenderPass();
        static void CreateFramebuffers();
        static void CreateGraphicsPipeline();
    };

}