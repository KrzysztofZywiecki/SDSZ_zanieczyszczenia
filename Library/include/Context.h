#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Device.h"
#include "BasicInfo.h"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>

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

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class Context
    {
        public:
            void InitVulkan(Window* window);
            void CleanUP();

            void DoTheThing(); //For test ONLY, delete later

        private:
            Window* window;
            Device device;
            Instance instance;

            SwapChainSupportDetails capabilities;
            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            std::vector<VkImageView> swapChainImageViews;
            std::vector<VkFramebuffer> framebuffers;
            VkPipeline graphicsPipeline;
            VkPipelineLayout graphicsPipelineLayout;
            VkRenderPass renderPass;
            VkCommandPool commandPool;
            VkPipeline computePipeline;
            
            VkSemaphore imageAcquiredSemaphore;
            VkSemaphore imageRenderedSemaphore;
            std::vector<VkCommandBuffer> commandBuffers;

            VkFormat swapChainImageFormat;
            VkExtent2D windowExtent;

            std::vector<const char*> GetRequiredExtensions();
            bool CheckValidationSupport();
            VkShaderModule CreateShaderModule(const char* path);

            VkPhysicalDevice PickPhysicalDevice();
            void CreateDevice();
            void CreateSwapChain(Window* window);
            void CreateSwapChainImageViews();
            void CreatePipelineLayout();
            void CreateRenderPass();
            void CreateFramebuffers();
            void CreateGraphicsPipeline();
            void CreateCommandPool();
            void RecordCommandBuffers();
            void CreateSyncObjects();
    };

}