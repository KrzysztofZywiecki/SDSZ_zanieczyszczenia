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
    }

    void Device::Destroy()
    {
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

    Buffer Device::CreateBuffer(void* data, size_t size, MemoryUsage usageHint, Ownership owner, VkBufferUsageFlags usage)
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        switch (usageHint)
        {
            case MemoryUsage::DYNAMIC:
            {
                VkBufferCreateInfo bufferInfo = {};
                bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                
                bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                bufferInfo.queueFamilyIndexCount = 0;
                bufferInfo.pQueueFamilyIndices = nullptr;
                bufferInfo.size = size;
                bufferInfo.usage = usage;
                
                vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);

                VkMemoryRequirements memoryRequirements = {};
                vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
                int32_t memoryIndex = ChooseMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                VkMemoryAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.memoryTypeIndex = memoryIndex;
                allocInfo.allocationSize = memoryRequirements.size;

                vkAllocateMemory(device, &allocInfo, nullptr, &memory);
                void* destination;
                vkMapMemory(device, memory, 0, size, 0, &destination);
                memcpy(destination, data, size);
                vkUnmapMemory(device, memory);
                vkBindBufferMemory(device, buffer, memory, 0);
            }
            break;
            default:
            {
                throw std::runtime_error("Unsupported usage hint used");
            }
            break;
        }
        return Buffer(buffer, memory, owner, usageHint);
    }

    void Device::DestroyBuffer(Buffer& buffer)
    {
        vkDestroyBuffer(device, buffer.buffer, nullptr);
        vkFreeMemory(device, buffer.memory, nullptr);
    }
}
