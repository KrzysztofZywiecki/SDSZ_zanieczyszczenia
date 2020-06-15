#include "Context.h"

namespace Library
{
    Context::Context()
    {

    }

    void Context::InitFreetype()
    {
        FT_Init_FreeType(&FreetypeLibrary);
    }

    void Context::InitVulkan(Window* window)
    {
        this-> window = window;
        instance.Create(*window);
        VkPhysicalDevice physicalDevice = PickPhysicalDevice();
        device.Create(physicalDevice, instance.GetInstance(), instance.GetSurface());
        
        CreateSwapChain(window);
        CreateSwapChainImageViews();
        CreateRenderPass();
        CreateFramebuffers();
        device.CreateCommandPools();
        RecordCommandBuffers();
        CreateSyncObjects();
    }

    void Context::CleanUP()
    {
        for(uint32_t i = 0; i < SIMULTANEOUS_FRAMES; i++)
        {
            vkDestroySemaphore(device.device, imageAcquiredSemaphores[i], nullptr);
            vkDestroySemaphore(device.device, imageRenderedSemaphores[i], nullptr);
            vkDestroySemaphore(device.device, computationFinishedSemaphores[i], nullptr);
            vkDestroyFence(device.device, presentationFinishedFences[i], nullptr);
        }
        vkFreeCommandBuffers(device.device, device.commandPools.graphicsPool, 
             static_cast<uint32_t>(renderingCommandBuffers.size()), renderingCommandBuffers.data());
        device.DestroyCommandPools();
        for(auto framebuffer : framebuffers)
        {
            vkDestroyFramebuffer(device.device, framebuffer, nullptr);
        }
        vkDestroyRenderPass(device.device, renderPass, nullptr);
        for(auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device.device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(device.device, swapChain, nullptr);
        device.Destroy();
        instance.Destroy();
    }

    VkPhysicalDevice Context::PickPhysicalDevice()
    {
        uint32_t count;
        vkEnumeratePhysicalDevices(instance.GetInstance(), &count, nullptr);
        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(instance.GetInstance(), &count, devices.data());
        VkPhysicalDevice physicalDevice;
        int index;

        int best_score = -1;
        int best_index = -1;
        for(index = 0; index < devices.size(); index++)
        {
            int score = IsDeviceSuitable(devices[index], instance.GetSurface());
            if(score > best_score)
            {
                best_score = score;
                best_index = index;
            }
        }
        if(best_index == -1)
        {
            throw std::runtime_error("Couldn't find suitable physical device");
        }
        physicalDevice = devices[best_index];
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        std::cout << properties.deviceName << std::endl;
        return physicalDevice;
     }

    static SwapChainSupportDetails querySwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
        details.presentModes.resize(count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, details.presentModes.data());

        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
        details.formats.resize(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, details.formats.data());
        
        return details;
    }

    static VkSurfaceFormatKHR chooseFormat(std::vector<VkSurfaceFormatKHR> formats)
    {
        for(auto format : formats)
        {
            if(format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && 
                format.format == VK_FORMAT_R8G8B8A8_SRGB)
                {
                    return format;
                }
        }
        return formats[0];
    }

    static VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> presentModes)
    {
        for(auto presentMode : presentModes)
        {
            if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D chooseSwapChainExtent(VkSurfaceCapabilitiesKHR capabilities, Window* window)
    {
        if(capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D newExtent = {window->getWidth(), window->getHeight()};
            
            newExtent.width = std::min(capabilities.maxImageExtent.width, 
                std::max(capabilities.minImageExtent.width, newExtent.width));
        
            newExtent.height = std::min(capabilities.maxImageExtent.height, 
                std::max(capabilities.minImageExtent.width, newExtent.height));
        }
        
    }

    void Context::CreateSwapChain(Window* window)
    {
		Context::capabilities = querySwapChainSupportDetails(device.physicalDevice, instance.GetSurface());
        VkSurfaceFormatKHR format = chooseFormat(capabilities.formats);
        VkExtent2D extent = chooseSwapChainExtent(capabilities.capabilities, window);
        
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = instance.GetSurface();
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.minImageCount = std::min(capabilities.capabilities.minImageCount + 1, capabilities.capabilities.maxImageCount);
        createInfo.imageFormat = format.format;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageArrayLayers = 1;
        createInfo.imageExtent = extent;
        createInfo.clipped = VK_TRUE;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        createInfo.presentMode = choosePresentMode(capabilities.presentModes);
        createInfo.preTransform = capabilities.capabilities.currentTransform;

		if (vkCreateSwapchainKHR(device.device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swapChain");
		}
        swapChainImageFormat = format.format;
        windowExtent = extent;

        uint32_t count;
        vkGetSwapchainImagesKHR(device.device, swapChain, &count, nullptr);
        swapChainImages.resize(count);
        vkGetSwapchainImagesKHR(device.device, swapChain, &count, swapChainImages.data());
    }

    void Context::CreateSwapChainImageViews()
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.format = swapChainImageFormat;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.levelCount = 1;

        swapChainImageViews.resize(swapChainImages.size());
        for(int i = 0; i < swapChainImages.size(); i++)
        {
            createInfo.image = swapChainImages[i];
            if(vkCreateImageView(device.device, &createInfo, nullptr, swapChainImageViews.data() + i) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image view");
            }
        }
    }

    static std::vector<char> readFile(const char* path)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.good())
		{
			std::string messege = "Failed to open file ";
			messege.append(path);
			throw std::runtime_error(messege);
		}
        size_t filesize = file.tellg();
        std::vector<char> fileContent(filesize);
        file.seekg(0);
        file.read(fileContent.data(), filesize);
        return fileContent;
    }

    VkShaderModule Context::CreateShaderModule(const char* path)
    {
        std::vector<char> shaderCode = readFile(path);
        VkShaderModule shaderModule;
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode.data());
        createInfo.codeSize = shaderCode.size();
        
        if(vkCreateShaderModule(device.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module");
        }
        return shaderModule;
    }

    void Context::CreateRenderPass()
    {
        VkAttachmentReference colorAttachment = {};
        colorAttachment.attachment = 0;
        colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription description = {};
        description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        description.format = swapChainImageFormat;
        description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        description.samples = VK_SAMPLE_COUNT_1_BIT;
        description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        VkSubpassDescription subpass = {};
        subpass.colorAttachmentCount = 1;
        subpass.inputAttachmentCount = 0;
        subpass.preserveAttachmentCount = 0;
        subpass.pColorAttachments = &colorAttachment;
        subpass.pDepthStencilAttachment = nullptr;
        subpass.pPreserveAttachments = nullptr;
        subpass.pResolveAttachments = nullptr;
        subpass.pInputAttachments = nullptr;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


        VkRenderPassCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.subpassCount = 1;
        createInfo.attachmentCount = 1;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;
        createInfo.pAttachments = &description;
        createInfo.pSubpasses = &subpass;

        if(vkCreateRenderPass(device.device, &createInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
    }

    void Context::CreateFramebuffers()
    {
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.layers = 1;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.width = windowExtent.width;
        framebufferInfo.height = windowExtent.height;
        framebufferInfo.renderPass = renderPass;

        framebuffers.resize(swapChainImageViews.size());

        for(uint32_t i = 0; i < swapChainImageViews.size(); i++)
        {
            framebufferInfo.pAttachments = swapChainImageViews.data() + i;
            if(vkCreateFramebuffer(device.device, &framebufferInfo, nullptr, framebuffers.data() + i) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create framebuffer");
            }
        }
    }

    void Context::RecordCommandBuffers()
    {
        VkCommandBufferAllocateInfo renderAllocInfo = {};
		renderAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		renderAllocInfo.commandBufferCount = framebuffers.size();
		renderAllocInfo.commandPool = device.commandPools.graphicsPool;
		renderAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkCommandBufferAllocateInfo compAllocInfo = {};
        compAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        compAllocInfo.commandBufferCount = framebuffers.size();
        compAllocInfo.commandPool = device.commandPools.computePool;
        compAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        renderingCommandBuffers.resize(framebuffers.size());
		computeCommandBuffers.resize(framebuffers.size());

        if(vkAllocateCommandBuffers(device.device, &renderAllocInfo, renderingCommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers");
        }
        if(vkAllocateCommandBuffers(device.device, &compAllocInfo, computeCommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate compute command buffer");
        }
    }

    void Context::BeginRendering()
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = nullptr;

        VkClearValue clearColor = {0.4, 0.4, 0.4, 1.0};

        VkRect2D renderArea = {};
        renderArea.extent = windowExtent;
        renderArea.offset = {0,0};

        VkRenderPassBeginInfo passInfo = {};
        passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        passInfo.renderPass = renderPass;
        passInfo.clearValueCount = 1;
        passInfo.pClearValues = &clearColor;
        passInfo.renderArea = renderArea;
        passInfo.framebuffer = framebuffers[imageIndex];

        vkBeginCommandBuffer(renderingCommandBuffers[imageIndex], &beginInfo);
        vkCmdBeginRenderPass(renderingCommandBuffers[imageIndex], &passInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void Context::EndRendering()
    {
        vkCmdEndRenderPass(renderingCommandBuffers[imageIndex]);
        vkEndCommandBuffer(renderingCommandBuffers[imageIndex]);
        
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = renderingCommandBuffers.data() + imageIndex;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &imageRenderedSemaphores[currentFrame];
        submitInfo.pWaitSemaphores = &imageAcquiredSemaphores[currentFrame];
    }

    void Context::CreateSyncObjects()
    {
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        imageAcquiredSemaphores.resize(SIMULTANEOUS_FRAMES);
        imageRenderedSemaphores.resize(SIMULTANEOUS_FRAMES);
        computationFinishedSemaphores.resize(SIMULTANEOUS_FRAMES);

        presentationFinishedFences.resize(SIMULTANEOUS_FRAMES);
        imageFences.resize(swapChainImages.size(), VK_NULL_HANDLE);

        for(uint32_t i = 0; i < SIMULTANEOUS_FRAMES; i++)
        {
            vkCreateSemaphore(device.device, &createInfo, nullptr, imageAcquiredSemaphores.data() + i);
            vkCreateSemaphore(device.device, &createInfo, nullptr, imageRenderedSemaphores.data() + i);
            vkCreateSemaphore(device.device, &createInfo, nullptr, computationFinishedSemaphores.data() + i);
            vkCreateFence(device.device, &fenceInfo, nullptr, presentationFinishedFences.data() + i);
        }
    }

    void Context::BeginFrame()
    {
        vkWaitForFences(device.device, 1, presentationFinishedFences.data() + currentFrame, VK_TRUE, UINT32_MAX);
        vkAcquireNextImageKHR(device.device, swapChain, UINT32_MAX, imageAcquiredSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    
        if(imageFences[imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(device.device, 1, imageFences.data() + imageIndex, VK_TRUE, UINT32_MAX);
        }
        imageFences[imageIndex] = presentationFinishedFences[currentFrame];
    }

    void Context::EndFrame()
    {
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        
        VkSemaphore waitSemaphores[] = {computationFinishedSemaphores[currentFrame]};

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pSignalSemaphores = &imageRenderedSemaphores[currentFrame];
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = renderingCommandBuffers.data() + imageIndex;

        vkResetFences(device.device, 1, presentationFinishedFences.data() + currentFrame);

        vkQueueSubmit(device.queues.graphicsQueue, 1, &submitInfo, presentationFinishedFences[currentFrame]);
    

        VkResult result;
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &imageRenderedSemaphores[currentFrame];
        presentInfo.pResults = &result;

        vkQueuePresentKHR(device.queues.presentQueue, &presentInfo);
    
        currentFrame = (currentFrame + 1) % SIMULTANEOUS_FRAMES;
    }

    void Context::BeginComputeOperations()
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = nullptr;
        
        vkBeginCommandBuffer(computeCommandBuffers[imageIndex], &beginInfo);
    }

    void Context::EndComputeOperations()
    {
        vkEndCommandBuffer(computeCommandBuffers[imageIndex]);
        
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAcquiredSemaphores[currentFrame];
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.pSignalSemaphores = &computationFinishedSemaphores[currentFrame];
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = computeCommandBuffers.data() + imageIndex;

        vkQueueSubmit(device.queues.computeQueue, 1, &submitInfo, VK_NULL_HANDLE);
    }
}