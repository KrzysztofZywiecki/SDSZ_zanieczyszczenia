#include "Context.h"

namespace Library
{
    Context::Context()
    {

    }

    void Context::InitVulkan(Window* window)
    {
        this-> window = window;
        instance.Create(*window);
        VkPhysicalDevice physicalDevice = PickPhysicalDevice();
        device.Create(physicalDevice, instance.GetInstance(), instance.GetSurface());
        
        std::vector<float> vertices = {-0.5f, -0.5f, 0.0f,
                                        0.5f, -0.5f, 0.0f,
                                        0.5f, 0.5f, 0.0f,
                                        -0.5f, 0.5f, 0.0f};
        std::vector<float> colors = {1.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f,
                                    1.0f, 1.0f, 1.0f};
        std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

        vertexBuffer = device.CreateBuffer(vertices.data(), vertices.size() * sizeof(float), 
        DYNAMIC, GRAPHICS, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        colorBuffer = device.CreateBuffer(colors.data(), colors.size() * sizeof(float), 
        DYNAMIC, GRAPHICS, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        indexBuffer = device.CreateBuffer(indices.data(), indices.size() * sizeof(unsigned int), DYNAMIC,
        GRAPHICS, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

        CreateSwapChain(window);
        CreateSwapChainImageViews();
        CreatePipelineLayout();
        CreateRenderPass();
        CreateFramebuffers();
        CreateGraphicsPipeline();
        device.CreateCommandPools();
        RecordCommandBuffers();
        CreateSyncObjects();
    }

    void Context::CleanUP()
    {
        vkDestroySemaphore(device.device, imageRenderedSemaphore, nullptr);
        vkDestroySemaphore(device.device, imageAcquiredSemaphore, nullptr);
        vkFreeCommandBuffers(device.device, device.commandPools.graphicsPool, commandBuffers.size(), commandBuffers.data());
        device.DestroyCommandPools();
        vkDestroyPipeline(device.device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device.device, graphicsPipelineLayout, nullptr);
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
        device.DestroyBuffer(vertexBuffer);
        device.DestroyBuffer(colorBuffer);
        device.DestroyBuffer(indexBuffer);
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
        for(auto device : devices)
        {
            if(IsDeviceSuitable(device, instance.GetSurface()) == 1000)
            {
                physicalDevice = device;
                break;
            }
        }
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

    void Context::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.pushConstantRangeCount = 0;
        createInfo.pPushConstantRanges = nullptr;
        createInfo.setLayoutCount = 0;
        createInfo.pSetLayouts = nullptr;
        if(vkCreatePipelineLayout(device.device, &createInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline layout");
        }
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
        description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
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

    void Context::CreateGraphicsPipeline()
    {
        VkShaderModule vertexShader = CreateShaderModule("Shaders/vert.spv");
        VkShaderModule fragmentShader = CreateShaderModule("Shaders/frag.spv");
        
        VkPipelineShaderStageCreateInfo vertexStage = {};
        VkPipelineShaderStageCreateInfo fragmentStage = {};

        vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStage.pName = "main";
        vertexStage.module = vertexShader;
        
        fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStage.pName = "main";
        fragmentStage.module = fragmentShader;

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertexStage, fragmentStage};

        VkPipelineColorBlendAttachmentState attachmentState = {};
        attachmentState.blendEnable = VK_FALSE;
        attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlend = {};
        colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlend.attachmentCount = 1;
        colorBlend.pAttachments = &attachmentState;
        colorBlend.logicOpEnable = VK_FALSE;
        colorBlend.logicOp = VK_LOGIC_OP_COPY;
        colorBlend.blendConstants[0] = 0.0f;
        colorBlend.blendConstants[1] = 0.0f;
        colorBlend.blendConstants[2] = 0.0f;
        colorBlend.blendConstants[3] = 0.0f;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.pSampleMask = nullptr;
        multisampling.minSampleShading = 1.0f;
        multisampling.alphaToOneEnable = VK_FALSE;
        multisampling.alphaToCoverageEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterization = {};
        rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.rasterizerDiscardEnable = VK_FALSE;
        rasterization.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization.lineWidth = 1.0f;
        rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization.depthClampEnable = VK_FALSE;
        rasterization.depthBiasEnable = VK_FALSE;
        rasterization.cullMode = VK_CULL_MODE_NONE;
        rasterization.depthBiasConstantFactor = 0.0f;
        rasterization.depthBiasClamp = 0.0f;
        rasterization.depthBiasSlopeFactor = 1.0f;

        VkVertexInputBindingDescription positionDescription = {};
        VkVertexInputBindingDescription colorDescription = {};
        positionDescription.binding = 0;
        positionDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        positionDescription.stride = 3*sizeof(float);

        colorDescription.binding = 1;
        colorDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        colorDescription.stride = 3*sizeof(float);

        VkVertexInputAttributeDescription positionInput = {};
        VkVertexInputAttributeDescription colorInput = {};
        positionInput.offset = 0;
        positionInput.location = 0;
        positionInput.format = VK_FORMAT_R32G32B32_SFLOAT;
        positionInput.binding = 0;
        colorInput.offset = 0;
        colorInput.location = 1;
        colorInput.format = VK_FORMAT_R32G32B32_SFLOAT;
        colorInput.binding = 1;

        VkVertexInputAttributeDescription attributes[] = {positionInput, colorInput};
        VkVertexInputBindingDescription bindings[] = {positionDescription, colorDescription};

        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexAttributeDescriptionCount = 2;
        vertexInput.vertexBindingDescriptionCount = 2;
        vertexInput.pVertexAttributeDescriptions = attributes;
        vertexInput.pVertexBindingDescriptions = bindings;

        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = (float)windowExtent.width;
        viewport.height = (float)windowExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = windowExtent;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.scissorCount = 1;
        viewportState.viewportCount = 1;
        viewportState.pScissors = &scissor;
        viewportState.pViewports = &viewport;

        VkGraphicsPipelineCreateInfo createInfo = {};
        createInfo.basePipelineHandle = graphicsPipeline;
        createInfo.basePipelineIndex = -1;
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStages;
        createInfo.layout = graphicsPipelineLayout;
        createInfo.renderPass = renderPass;
        createInfo.subpass = 0;
        createInfo.pColorBlendState = &colorBlend;
        createInfo.pDepthStencilState = nullptr;
        createInfo.pDynamicState = nullptr;
        createInfo.pInputAssemblyState = &inputAssembly;
        createInfo.pMultisampleState = &multisampling;
        createInfo.pRasterizationState = &rasterization;        
        createInfo.pTessellationState = nullptr;
        createInfo.pVertexInputState = &vertexInput;
        createInfo.pViewportState = &viewportState;

        if(vkCreateGraphicsPipelines(device.device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline, debilu");
        }
        vkDestroyShaderModule(device.device, vertexShader, nullptr);
        vkDestroyShaderModule(device.device, fragmentShader, nullptr);
    }

    void Context::RecordCommandBuffers()
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandBufferCount = framebuffers.size();
        allocInfo.commandPool = device.commandPools.graphicsPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        
        commandBuffers.resize(framebuffers.size());

        if(vkAllocateCommandBuffers(device.device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers");
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = nullptr;

        VkClearValue clearColor = {0.0, 0.0, 0.0, 1.0};

        VkRect2D renderArea = {};
        renderArea.extent = windowExtent;
        renderArea.offset = {0,0};

        VkRenderPassBeginInfo passInfo = {};
        passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        passInfo.renderPass = renderPass;
        passInfo.pClearValues = &clearColor;
        passInfo.clearValueCount = 1;
        passInfo.renderArea = renderArea;

        for(uint32_t i = 0; i < commandBuffers.size(); i++)
        {
            passInfo.framebuffer = framebuffers[i];

            vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
            vkCmdBeginRenderPass(commandBuffers[i], &passInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            
            VkBuffer buffers[] = {vertexBuffer.buffer, colorBuffer.buffer};
            VkDeviceSize offsets[] = {0, 0};
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 2, buffers, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffers[i], 6, 1, 0, 0, 0);
            vkCmdEndRenderPass(commandBuffers[i]);
            vkEndCommandBuffer(commandBuffers[i]);
        }
    }

    void Context::CreateSyncObjects()
    {
        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        vkCreateSemaphore(device.device, &createInfo, nullptr, &imageRenderedSemaphore);
        vkCreateSemaphore(device.device, &createInfo, nullptr, &imageAcquiredSemaphore);
    }

    void Context::DoTheThing()
    {
        uint32_t index;
        vkAcquireNextImageKHR(device.device, swapChain, UINT32_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &index);

        VkPipelineStageFlags waitDstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers.data() + index;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &imageRenderedSemaphore;
        submitInfo.pWaitDstStageMask = &waitDstStage;

        vkQueueSubmit(device.queues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &imageRenderedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain;
        presentInfo.pImageIndices = &index;

        vkQueuePresentKHR(device.queues.graphicsQueue, &presentInfo);
        vkDeviceWaitIdle(device.device);
    }

}