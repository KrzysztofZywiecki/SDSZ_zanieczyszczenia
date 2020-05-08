#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Window.h"
#include "Device.h"
#include "BasicInfo.h"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "ft2build.h"
#include FT_FREETYPE_H

#define SIMULTANEOUS_FRAMES 2

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
            void InitFreetype();
            void InitVulkan(Window* window);
            void CleanUP();

            void BeginRendering();
            void EndRendering();
            void BeginFrame();
            void EndFrame();

            void BeginComputeOperations();
            void EndComputeOperations();
        
            VkCommandBuffer GetComputeBuffer(){return computeCommandBuffers[imageIndex];}
            VkCommandBuffer GetRenderingBuffer(){return renderingCommandBuffers[imageIndex];}
           
            VkShaderModule CreateShaderModule(const char* path);

            Device device;
            VkRenderPass renderPass;
            VkExtent2D windowExtent;

            std::vector<VkFence> presentationFinishedFences;

            VkCommandPool GetGraphicsPool(){return device.commandPools.graphicsPool;};
            VkCommandPool GetComputePool(){return device.commandPools.computePool;};

        private:
            FT_Library FreetypeLibrary;

            Window* window;
            Instance instance;

            SwapChainSupportDetails capabilities;
            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            std::vector<VkImageView> swapChainImageViews;
            std::vector<VkFramebuffer> framebuffers;


            std::vector<VkSemaphore> imageAcquiredSemaphores;
            std::vector<VkSemaphore> imageRenderedSemaphores;
            std::vector<VkSemaphore> computationFinishedSemaphores;

            std::vector<VkFence> imageFences;

            std::vector<VkCommandBuffer> renderingCommandBuffers;
            std::vector<VkCommandBuffer> computeCommandBuffers;

            uint32_t imageIndex;
            uint32_t currentFrame = 0;

            VkFormat swapChainImageFormat;

            VkPhysicalDevice PickPhysicalDevice();
            void CreateDevice();
            void CreateSwapChain(Window* window);
            void CreateSwapChainImageViews();
            void CreateRenderPass();
            void CreateFramebuffers();
            void RecordCommandBuffers();
            void CreateSyncObjects();
    };

}