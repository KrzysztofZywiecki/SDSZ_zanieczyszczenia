#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Buffer.h"
#include <stdexcept>
#include <vector>
#include <optional>
#include <set>


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
            void CreateCommandPools();
            void DestroyCommandPools();
        private:
    };

}