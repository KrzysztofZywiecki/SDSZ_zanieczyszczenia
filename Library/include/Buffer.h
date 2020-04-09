#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Enums.h"

namespace Library
{

    int32_t ChooseMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);
    

    struct Buffer
    {
            Buffer();
            Buffer(VkBuffer buffer, VkDeviceMemory memory,
                Ownership owner, MemoryUsage usage, size_t size);
            
            VkBuffer buffer;
            VkDeviceMemory memory;

            Ownership owner;
            MemoryUsage usage;
            size_t size;
        };

}

