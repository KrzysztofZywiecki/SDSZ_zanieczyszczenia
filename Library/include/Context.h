#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Device.h"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>

#ifndef EXT_LISTS
#define EXT_LISTS
    #ifdef NDEBUG
        static bool validationEnabled = false;
    #else
        static bool validationEnabled = true;
    #endif

    static std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    static std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif

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

        static void DoTheThing(); //For test ONLY, delete later

        static Device device;

        private:
        static VkDebugUtilsMessengerEXT debugMessenger;
        static VkInstance instance;
        static VkSurfaceKHR surface;
        static SwapChainSupportDetails capabilities;
        static VkSwapchainKHR swapChain;
        static std::vector<VkImage> swapChainImages;
        static std::vector<VkImageView> swapChainImageViews;
        static std::vector<VkFramebuffer> framebuffers;
        static VkPipeline graphicsPipeline;
        static VkPipelineLayout graphicsPipelineLayout;
        static VkRenderPass renderPass;
        static VkCommandPool commandPool;
        static VkSemaphore imageAcquiredSemaphore;
        static VkSemaphore imageRenderedSemaphore;
        static std::vector<VkCommandBuffer> commandBuffers;

        static VkFormat swapChainImageFormat;
        static VkExtent2D windowExtent;

        static std::vector<const char*> GetRequiredExtensions();
        static bool CheckValidationSupport();
        static VkShaderModule CreateShaderModule(const char* path);

        static void CreateInstance();
        static void SetupDebugMessenger();
        static void CreateSurface(GLFWwindow* window);
        static VkPhysicalDevice PickPhysicalDevice();
        static void CreateDevice();
        static void CreateSwapChain(Window* window);
        static void CreateSwapChainImageViews();
        static void CreatePipelineLayout();
        static void CreateRenderPass();
        static void CreateFramebuffers();
        static void CreateGraphicsPipeline();
        static void CreateCommandPool();
        static void RecordCommandBuffers();
        static void CreateSyncObjects();
    };

}