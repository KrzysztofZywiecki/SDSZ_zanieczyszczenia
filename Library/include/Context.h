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
            Buffer uniformBuffer;
            Image image;

            SwapChainSupportDetails capabilities;
            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            std::vector<VkImageView> swapChainImageViews;
            std::vector<VkFramebuffer> framebuffers;
            VkRenderPass renderPass;
            VkPipeline graphicsPipeline = VK_NULL_HANDLE;
            VkPipelineLayout graphicsPipelineLayout;
            VkPipelineLayout computePipelineLayout;
            VkPipeline computePipeline = VK_NULL_HANDLE;
            VkDescriptorSetLayout setLayout;
            VkDescriptorSetLayout textureLayout;
            VkDescriptorPool descriptorPool;
            VkDescriptorSet descriptorSet;
            VkDescriptorSet texture;
            VkDescriptorSetLayout storageImageLayout;
            VkDescriptorSet storageImage;
            
            VkSemaphore imageAcquiredSemaphore;
            VkSemaphore imageRenderedSemaphore;
            VkSemaphore computationFinishedSemaphore;
            std::vector<VkCommandBuffer> commandBuffers;
            VkCommandBuffer dispatchComputeCommandBuffer;


            VkFormat swapChainImageFormat;
            VkExtent2D windowExtent;

            std::vector<const char*> GetRequiredExtensions();
            bool CheckValidationSupport();
            VkShaderModule CreateShaderModule(const char* path);

            VkPhysicalDevice PickPhysicalDevice();
            void CreateDevice();
            void CreateSwapChain(Window* window);
            void CreateSwapChainImageViews();
            void CreateRenderPass();
            void CreateGraphicsPipelineLayout();
            void CreateComputePipelineLayout();
            void CreateGraphicsPipeline();
            void CreateComputePipeline();
            void CreateFramebuffers();
            void RecordCommandBuffers();
            void CreateSyncObjects();
            void CreateDescriptorSetLayout();
            void CreateDescriptorPool();
            void CreateDescriptorSet();
    };

}