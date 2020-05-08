#include "Device.h"

namespace Library
{

    Device::Device() : device(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE)
    {
    }

    void Device::Create(VkPhysicalDevice physicalDevice, VkInstance instance, VkSurfaceKHR surface)
    {
        this->physicalDevice = physicalDevice;
        indices = GetQueueFamilyIndices(physicalDevice, surface);
        std::set<uint32_t> uniqueIndices = {indices.graphicsFamily.value(), indices.computeFamily.value(), indices.presentFamily.value()};
        std::vector<VkDeviceQueueCreateInfo> queueInfos(uniqueIndices.size());
        uint32_t i = 0;
        for(auto index : uniqueIndices)
        {
            float priority = 1.0f;
            queueInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfos[i].queueFamilyIndex = index;
            queueInfos[i].queueCount = 1;
            queueInfos[i].pQueuePriorities = &priority;
            i++;
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = queueInfos.size();
        createInfo.pQueueCreateInfos = queueInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        if(validationEnabled)
        {
            createInfo.enabledLayerCount = layers.size();
            createInfo.ppEnabledLayerNames = layers.data();
        }
        else
        {
            createInfo.ppEnabledLayerNames = nullptr;
            createInfo.enabledLayerCount = 0;
        }

        vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
        
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &queues.graphicsQueue);
        vkGetDeviceQueue(device, indices.computeFamily.value(), 0, &queues.computeQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &queues.presentQueue);
    
        InitImages();
    }

    void Device::InitImages()
    {
        VkDescriptorPoolSize poolSizes[2 * MAX_TEXTURES];
        for(uint32_t i = 0; i < MAX_TEXTURES; i++)
        {
            poolSizes[2 * i].descriptorCount = 1;
            poolSizes[2 * i].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

            poolSizes[2 * i + 1].descriptorCount = 1;
            poolSizes[2 * i + 1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 2 * MAX_TEXTURES;
        poolInfo.pPoolSizes = poolSizes;
        poolInfo.maxSets = 2 * MAX_TEXTURES;
        if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &texturePool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image descriptor pool");
        }

        VkDescriptorSetLayoutBinding storageBinding = {};
        storageBinding.binding = 0;
        storageBinding.descriptorCount = 1;
        storageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        storageBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        storageBinding.pImmutableSamplers = nullptr;
        VkDescriptorSetLayoutCreateInfo storageLayoutInfo = {};
        storageLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        storageLayoutInfo.bindingCount = 1;
        storageLayoutInfo.pBindings = &storageBinding;

        VkDescriptorSetLayoutBinding samplerBinding = {};
        samplerBinding.binding = 0;
        samplerBinding.descriptorCount = 1;
        samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerBinding.pImmutableSamplers = nullptr;
        VkDescriptorSetLayoutCreateInfo samplerLayoutInfo = {};
        samplerLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        samplerLayoutInfo.bindingCount = 1;
        samplerLayoutInfo.pBindings = &samplerBinding;

        if(vkCreateDescriptorSetLayout(device, &storageLayoutInfo, nullptr, &storageLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create storage image layout");
        }
        if(vkCreateDescriptorSetLayout(device, &samplerLayoutInfo, nullptr, &samplerLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create sampled image layout");
        }

        textureCount = 0;
    }

    void Device::Destroy()
    {
        vkDestroyDescriptorPool(device, texturePool, nullptr);
        vkDestroyDescriptorSetLayout(device, storageLayout, nullptr);
        vkDestroyDescriptorSetLayout(device, samplerLayout, nullptr);
        vkDestroyDevice(device, nullptr);
    }

    Device::~Device()
    {
    }

    QueueFamilyIndices GetQueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint32_t count;
        QueueFamilyIndices indices = {};
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
        std::vector<VkQueueFamilyProperties> propertyVector(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, propertyVector.data());

        for(uint32_t i = 0; i < count; i++)
        {
            VkBool32 supported = false;
            if((propertyVector[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !indices.graphicsFamily.has_value())
            {
                indices.graphicsFamily = i;
            }
            if((propertyVector[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && !indices.computeFamily.has_value())
            {
                indices.computeFamily = i;
            }
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supported);
            if(supported && !indices.presentFamily.has_value())
            {
                indices.presentFamily = i;
            }
        }
        return indices;
    }

    static bool checkExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t count;
        std::vector<VkExtensionProperties> properties;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
        properties.resize(count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, properties.data());

		for (auto name : deviceExtensions)
		{
			bool extensionFound = false;
			for (auto property : properties)
			{
				if (strcmp(property.extensionName, name) == 0)
				{
					extensionFound = true;
					break;
				}
			}
			if (!extensionFound)
			{
				return false;
			}
		}
		return true;
    }

    uint32_t IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices = GetQueueFamilyIndices(device, surface);

        if(!indices.IsComplete() || !checkExtensionSupport(device))
        {
            return 0;
        }
        else
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                return 1000;
            }
            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                return 500;
            }
            return 100;
        }
    }

    VkDeviceMemory Device::AllocateMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags propertyFlags)
    {
        VkDeviceMemory memory;
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.memoryTypeIndex = ChooseMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, propertyFlags);
        if(allocInfo.memoryTypeIndex == -1)
        {
            throw std::runtime_error("Failed to find suitable memory type");
        }
        allocInfo.allocationSize = memoryRequirements.size;

        if(vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate device memory");
        }
        return memory;
    }

    Buffer Device::CreateBuffer(void* data, size_t size, MemoryUsage usageHint, Ownership owner, VkBufferUsageFlags usage)
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = 0;
        bufferInfo.pQueueFamilyIndices = nullptr;
        bufferInfo.size = size;
        
        switch (usageHint)
        {
            case MemoryUsage::DYNAMIC:
            {
                bufferInfo.usage = usage;
                vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
                VkMemoryRequirements memoryRequirements = {};
                vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
                memory = AllocateMemory(memoryRequirements, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				if (data != nullptr)
				{
					void* destination;
					vkMapMemory(device, memory, 0, size, 0, &destination);
					memcpy(destination, data, size);
					vkUnmapMemory(device, memory);
				}
				vkBindBufferMemory(device, buffer, memory, 0);
            }
            break;
            case STATIC:
            {
                bufferInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
        
                Buffer stagingBuffer = CreateBuffer(data, size, DYNAMIC, owner, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
                VkMemoryRequirements memoryRequirements = {};
                vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
                memory = AllocateMemory(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				vkBindBufferMemory(device, buffer, memory, 0);
                VkCommandBuffer singleTimeCommand = BeginSingleTimeCommand(owner);
                VkBufferCopy region = {};
                region.size = size;
                region.srcOffset = 0;
                region.dstOffset = 0;
                vkCmdCopyBuffer(singleTimeCommand, stagingBuffer.buffer, buffer, 1, &region);
                EndSingleTimeCommand(singleTimeCommand, owner);
                DestroyBuffer(stagingBuffer);
				break;
            }
            default:
            {
                throw std::runtime_error("Unsupported usage hint used");
            }
            break;
        }
        return Buffer(buffer, memory, owner, usageHint, size);
    }

    void Device::AssignMemory(Buffer buffer, void* data, size_t size)
    {
        if(size > buffer.size)
        {
            throw std::runtime_error("Requested assign size greater than buffer original size");
        }
        switch(buffer.usage)
        {
            case DYNAMIC:
                void* bufData;
                vkMapMemory(device, buffer.memory, 0, size, 0, &bufData);
                memcpy(bufData, data, size);
                vkUnmapMemory(device, buffer.memory);
                break;
            default:
                throw std::runtime_error("Memory usage not supported");
                break;
        }
    }
    void Device::DestroyBuffer(Buffer& buffer)
    {
        vkDestroyBuffer(device, buffer.buffer, nullptr);
        vkFreeMemory(device, buffer.memory, nullptr);
    }

    void Device::CreateImageBindings(Image& image)
    {
        if(textureCount >= MAX_TEXTURES)
        {
            throw std::runtime_error("Max texture count exceeded!");
        }
        VkDescriptorSetLayout layouts[] = {storageLayout, samplerLayout};

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pSetLayouts = layouts;
        allocInfo.descriptorSetCount = 2;
        allocInfo.descriptorPool = texturePool;
        VkDescriptorSet sets[2] = {VK_NULL_HANDLE, VK_NULL_HANDLE};
        if(vkAllocateDescriptorSets(device, &allocInfo, sets) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate texture descriptor sets");
        }

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageInfo.imageView = image.imageView;
        imageInfo.sampler = image.sampler;

        VkWriteDescriptorSet storageWrite = {};
        storageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        storageWrite.pTexelBufferView = nullptr;
        storageWrite.pImageInfo = &imageInfo;
        storageWrite.pBufferInfo = nullptr;
        storageWrite.dstBinding = 0;
        storageWrite.dstArrayElement = 0;
        storageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        storageWrite.descriptorCount = 1;
        storageWrite.dstSet = sets[0];

        VkWriteDescriptorSet samplerWrite = {};
        samplerWrite = storageWrite;
        samplerWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerWrite.dstSet = sets[1];

        VkWriteDescriptorSet writes[] = {storageWrite, samplerWrite};
        vkUpdateDescriptorSets(device, 2, writes, 0, nullptr);

        image.storageBinding = sets[0];
        image.samplerBinding = sets[1];
        image.creationIndex = textureCount;
        textureCount ++;
    }

    Image Device::CreateImage(VkImageAspectFlags aspect, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, Ownership owner, void* data, size_t size)
    {
        Image image;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.extent = VkExtent3D({width, height, 1});
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.format = format;

        if(vkCreateImage(device, &imageInfo, nullptr, &image.image) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Image object");
        }

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, image.image, &memoryRequirements);
        image.memory = AllocateMemory(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindImageMemory(device, image.image, image.memory, 0);

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;    
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.format = format;
        viewInfo.image = image.image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.aspectMask = aspect;
        if(vkCreateImageView(device, &viewInfo, nullptr, &image.imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image view");
        }

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16.0;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.maxLod = 0.0;
        samplerInfo.minLod = 0.0;
        samplerInfo.mipLodBias = 0.0;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        if(vkCreateSampler(device, &samplerInfo, nullptr, &image.sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create sampler");
        }

        if(data != nullptr)
        {
            Buffer imageBuffer = CreateBuffer(data, width * height * size, DYNAMIC, owner, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            TransitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, GRAPHICS);
            
            VkBufferImageCopy region = {};
            region.bufferImageHeight = height;
            region.bufferRowLength = width;
            region.bufferOffset = 0;
            region.imageOffset = VkOffset3D({0,0,0});
            region.imageExtent = VkExtent3D({width, height, 1});
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageSubresource.mipLevel = 0;
            VkCommandBuffer commandBuffer = BeginSingleTimeCommand(owner);
            vkCmdCopyBufferToImage(commandBuffer, imageBuffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
            EndSingleTimeCommand(commandBuffer, owner);
            TransitionImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, owner);
            DestroyBuffer(imageBuffer);
        }

        CreateImageBindings(image);
        return image;
    }

    Image Device::CreateWriteOnlyImage(VkImageAspectFlags aspect, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, size_t size)
    {
        Image image;

        VkImageCreateInfo imageInfo = {};

        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = VkExtent3D({width, height, 1});
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

        if(vkCreateImage(device, &imageInfo, nullptr, &image.image) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image");
        }

        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(device, image.image, &requirements);
        image.memory = AllocateMemory(requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindImageMemory(device, image.image, image.memory, 0);

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.image = image.image;
        viewInfo.format = format;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.subresourceRange.aspectMask = aspect;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.levelCount = 1;

        if(vkCreateImageView(device, &viewInfo, nullptr, &image.imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Image View");
        }

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        samplerInfo.maxAnisotropy = 16.0f;
        samplerInfo.maxLod = 0.0;
        samplerInfo.minLod = 0.0;
        samplerInfo.mipLodBias = 0.0;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        
        if(vkCreateSampler(device, &samplerInfo, nullptr, &image.sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image sampler");
        }

        image.owner = COMPUTE;
        TransitionImageLayout(image, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_GENERAL, COMPUTE);
        
        CreateImageBindings(image);
        return image;
    }

    void Device::CreateCommandPools()
    {
        std::set uniqueIndices = {indices.graphicsFamily.value(), indices.computeFamily.value()};
        for(auto index : uniqueIndices)
        {
            VkCommandPool commandPool;
            VkCommandPoolCreateInfo poolInfo = {};
            poolInfo.queueFamilyIndex = index;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);

            if(indices.graphicsFamily == index)
            {
                commandPools.graphicsPool = commandPool;
            }
            if(indices.computeFamily == index)
            {
                commandPools.computePool = commandPool;
            }
        }        
    }

    void Device::DestroyCommandPools()
    {
        std::set<VkCommandPool> uniqueCommandPools = {commandPools.graphicsPool, commandPools.computePool};
        for(auto pool : uniqueCommandPools)
        {
            vkDestroyCommandPool(device, pool, nullptr);
        }
    }

    VkCommandBuffer Device::BeginSingleTimeCommand(Ownership family)
    {
        VkCommandBuffer commandBuffer;
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        switch(family)
        {
            case GRAPHICS:
                allocInfo.commandPool = commandPools.graphicsPool;
            break;
            case COMPUTE:
                allocInfo.commandPool = commandPools.computePool;
            break;
        }
        if(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create temporary command buffer");
        }
		VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = nullptr;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void Device::EndSingleTimeCommand(VkCommandBuffer commandBuffer, Ownership owner)
    {
        VkCommandPool commandPool;
        VkQueue queue;
        switch(owner)
        {
            case GRAPHICS:
            commandPool = commandPools.graphicsPool;
            queue = queues.graphicsQueue;
            break;
            case COMPUTE:
            commandPool = commandPools.computePool;
            queue = queues.computeQueue;
        }
        vkEndCommandBuffer(commandBuffer);
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.pWaitDstStageMask = VK_NULL_HANDLE;

        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void Device::TransitionImageLayout(Image image, VkImageLayout oldLayout, VkImageLayout newLayout, Ownership newOwner)
    {
        VkImageMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        memoryBarrier.image = image.image;
        memoryBarrier.oldLayout = oldLayout;
        memoryBarrier.newLayout = newLayout;
        memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        memoryBarrier.subresourceRange.baseArrayLayer = 0;
        memoryBarrier.subresourceRange.baseMipLevel = 0;
        memoryBarrier.subresourceRange.layerCount = 1;
        memoryBarrier.subresourceRange.levelCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            memoryBarrier.srcAccessMask = 0;
            memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			image.owner = newOwner;
        }
        else if(oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
        {
            memoryBarrier.srcAccessMask = 0;
            memoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

			memoryBarrier.dstQueueFamilyIndex = indices.computeFamily.value();
			memoryBarrier.srcQueueFamilyIndex = indices.computeFamily.value();

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			image.owner = newOwner;
        }
        else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL)
        {
            memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

            switch(newOwner)
            {
                case GRAPHICS:
                    memoryBarrier.srcQueueFamilyIndex = indices.graphicsFamily.value();
                    memoryBarrier.dstQueueFamilyIndex = indices.graphicsFamily.value();
                    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
                case COMPUTE:
                    memoryBarrier.srcQueueFamilyIndex = indices.computeFamily.value();
                    memoryBarrier.dstQueueFamilyIndex = indices.computeFamily.value();
                    destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                break;
                default:
                    throw std::runtime_error("You what mate?");
                break;
            }
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			image.owner = newOwner;
        }
        else if(oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_GENERAL)
        {
            if(image.owner != newOwner && queues.computeQueue == queues.graphicsQueue)
            {
                memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

                switch(newOwner)
                {
                    case GRAPHICS:
                        memoryBarrier.srcQueueFamilyIndex = indices.computeFamily.value();
                        memoryBarrier.dstQueueFamilyIndex = indices.graphicsFamily.value();
                        sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                    break;
                    case COMPUTE:
                        memoryBarrier.srcQueueFamilyIndex = indices.graphicsFamily.value();
                        memoryBarrier.dstQueueFamilyIndex = indices.computeFamily.value();
                        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                        destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                    break;
                    default:
                        throw std::runtime_error("You what mate?");
                    break;
                }
            }
			image.owner = newOwner;
        }
        VkCommandBuffer commandBuffer = BeginSingleTimeCommand(COMPUTE);
        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
        EndSingleTimeCommand(commandBuffer, COMPUTE);
    }

    void Device::DestroyImage(Image image)
    {
        vkDestroySampler(device, image.sampler, nullptr);
        vkDestroyImageView(device, image.imageView, nullptr);
        vkDestroyImage(device, image.image, nullptr);
        vkFreeMemory(device, image.memory, nullptr);
    }

}
