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
            VkImage image;
            VkImageView imageView;
            VkSampler sampler;
            VkDeviceMemory memory;

            VkDescriptorSet storageBinding;
            VkDescriptorSet samplerBinding;

            uint32_t creationIndex;
    };

}

