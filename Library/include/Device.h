#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Buffer.h"
#include "Image.h"
#include <stdexcept>
#include <vector>
#include <optional>
#include <set>

#define MAX_TEXTURES 20

#ifndef EXT_LISTS
#define EXT_LISTS
    #ifdef NDEBUG
        static bool validationEnabled = true;
    #else
        static bool validationEnabled = true;
    #endif

    static std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    static std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#endif

namespace Library
{

    uint32_t IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> computeFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() {return graphicsFamily.has_value() 
        && computeFamily.has_value() && presentFamily.has_value();}
    };

    struct Queues
    {
        VkQueue graphicsQueue;
        VkQueue computeQueue;
        VkQueue presentQueue;
    };

    struct CommandPools
    {
        VkCommandPool graphicsPool;
        VkCommandPool computePool;
    };

    QueueFamilyIndices GetQueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkPhysicalDevice PickPhysicalDevice(std::vector<VkPhysicalDevice> devices);

    class Device
    {
        public:
            VkDevice device;
            QueueFamilyIndices indices;
            Queues queues;
            CommandPools commandPools;
            VkPhysicalDevice physicalDevice;
            
            Device();
            ~Device();

            void Create(VkPhysicalDevice physicalDevice, VkInstance instance, VkSurfaceKHR surface);
			void Destroy();

            Buffer CreateBuffer(void* data, size_t size, MemoryUsage usageHint, Ownership owner, VkBufferUsageFlags usage);
            void DestroyBuffer(Buffer& buffer);
            void AssignMemory(Buffer buffer, void* data, size_t size);

            Image CreateImage(VkImageAspectFlags aspect, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, Ownership owner, void* data = nullptr, size_t size = 0);
            Image CreateWriteOnlyImage(VkImageAspectFlags aspect, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, size_t size);
            void DestroyImage(Image image);
            void CreateCommandPools();
            void DestroyCommandPools();
            VkCommandBuffer BeginSingleTimeCommand(Ownership family);
            void EndSingleTimeCommand(VkCommandBuffer commandBuffer, Ownership family);
            void PassOwnership(Buffer buffer, Ownership newOwner);
            void PassOwnership(Image image, Ownership newOwner);
            void TransitionImageLayout(Image image, VkImageLayout oldLayout, VkImageLayout newLayout, Ownership newOwner);
        
            VkDescriptorSetLayout GetStorageLayout(){return storageLayout;}
            VkDescriptorSetLayout GetSamplerLayout(){return samplerLayout;};
        private:
            VkDeviceMemory AllocateMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags propertyFlags);
            void InitImages();
            void CreateImageBindings(Image& image, VkImageUsageFlags usage);

            VkDescriptorPool texturePool;
            VkDescriptorSetLayout storageLayout;
            VkDescriptorSetLayout samplerLayout;

            uint32_t textureCount;
    };
}