#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Enums.h"

namespace Library
{

    struct Image
    {
        Image();

        Ownership owner;
        MemoryUsage usage;
        VkImage image;
        VkImageView imageView;
        VkDeviceMemory memory;
    };

}

