#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Library
{
    enum Ownership{GRAPHICS, COMPUTE, PRESENT};
    enum MemoryUsage{STATIC, DYNAMIC};

    int32_t ChooseMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);
    

    struct Buffer
    {
            Buffer();
            Buffer(VkBuffer buffer, VkDeviceMemory memory,
                Ownership owner, MemoryUsage usage);
            
            VkBuffer buffer;
            VkDeviceMemory memory;

            Ownership owner;
            MemoryUsage usage;
        };

}

