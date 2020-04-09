#include "Buffer.h"

namespace Library
{
    Buffer::Buffer()
    {
        
    }

    Buffer::Buffer(VkBuffer buffer, VkDeviceMemory memory,
        Ownership owner, MemoryUsage usage, size_t size)
        : buffer(buffer), memory(memory), owner(owner), usage(usage), size(size)
        {}

    int32_t ChooseMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties = {};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        for(int32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if((properties & memoryProperties.memoryTypes[i].propertyFlags) == properties &&
                memoryTypeBits & (1 << i))
            {
                return i;
            }
        }
        return -1;
    }

}