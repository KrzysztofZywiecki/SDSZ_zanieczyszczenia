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
            Context();
            void InitVulkan(Window* window);
            void CleanUP();

            void DoTheThing(); //For test ONLY, delete later


        private:
            Window* window;
            Device device;
            Instance instance;

            Buffer vertexBuffer;
            Buffer indexBuffer;
            Buffer colorBuffer;

            SwapChainSupportDetails capabilities;
            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            std::vector<VkImageView> swapChainImageViews;
            std::vector<VkFramebuffer> framebuffers;
            VkPipeline graphicsPipeline = VK_NULL_HANDLE;
            VkPipelineLayout graphicsPipelineLayout;
            VkRenderPass renderPass;
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
            void RecordCommandBuffers();
            void CreateSyncObjects();
    };

}