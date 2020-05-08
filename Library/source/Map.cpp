#include "Map.h"

namespace Library
{

    Map::~Map()
    {

    }

	void Map::CleanUp()
	{
		VkDevice device = context->device.device;
		context->device.DestroyBuffer(vertexBuffer);
		context->device.DestroyBuffer(indexBuffer);
		context->device.DestroyImage(images[0]);
		context->device.DestroyImage(images[1]);

		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);
		vkDestroyPipeline(device, computePipeline, nullptr);
		vkDestroyPipelineLayout(device, computePipelineLayout, nullptr);
	}

    Map::Map(Context* context, uint32_t width, uint32_t height, void* data, VkFormat format, size_t pixelSize)
        :context(context), width(width), height(height), pixelSize(pixelSize)
    {
        images[0] = context->device.CreateWriteOnlyImage(VK_IMAGE_ASPECT_COLOR_BIT, format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, width, height, pixelSize);
        images[1] = context->device.CreateImage(VK_IMAGE_ASPECT_COLOR_BIT, format, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, width, height, COMPUTE, data, pixelSize);
        CreateVulkanObjects();
    }

    void Map::DispatchCompute(bool acquireData)
    {
        vkCmdBindPipeline(context->GetComputeBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
        VkDescriptorSet sets[2];
        if(imageIndex == 0)
        {
            sets[0] = images[1].storageBinding;
            sets[1] = images[0].storageBinding;
        }
        else
        {
            sets[0] = images[0].storageBinding;
            sets[1] = images[1].storageBinding;
        }
        vkCmdBindDescriptorSets(context->GetComputeBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 2, sets, 0, nullptr);
        vkCmdDispatch(context->GetComputeBuffer(), width/16, height/16, 1);
        VkImageMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        memoryBarrier.image = images[imageIndex].image;
        memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        memoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
        memoryBarrier.srcQueueFamilyIndex = context->device.indices.computeFamily.value();
        memoryBarrier.dstQueueFamilyIndex = context->device.indices.graphicsFamily.value();
        memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        memoryBarrier.subresourceRange.baseArrayLayer = 0;
        memoryBarrier.subresourceRange.baseMipLevel = 0;
        memoryBarrier.subresourceRange.layerCount = 1;
        memoryBarrier.subresourceRange.levelCount = 1;
        vkCmdPipelineBarrier(context->GetComputeBuffer(), VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
    }

    void Map::Render()
    {
        uint8_t index = imageIndex ? 0 : 1;
        vkCmdBindPipeline(context->GetRenderingBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        vkCmdBindDescriptorSets(context->GetRenderingBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 0, 1, &(images + index)->samplerBinding, 0, nullptr);
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(context->GetRenderingBuffer(), 0, 1, &vertexBuffer.buffer, &offset);
        vkCmdBindIndexBuffer(context->GetRenderingBuffer(), indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(context->GetRenderingBuffer(), 6, 1, 0, 0, 0);
        
        VkImageMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        memoryBarrier.image = images[imageIndex].image;
        memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        memoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        memoryBarrier.srcQueueFamilyIndex = context->device.indices.graphicsFamily.value();
        memoryBarrier.dstQueueFamilyIndex = context->device.indices.computeFamily.value();
        memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        memoryBarrier.subresourceRange.baseArrayLayer = 0;
        memoryBarrier.subresourceRange.baseMipLevel = 0;
        memoryBarrier.subresourceRange.layerCount = 1;
        memoryBarrier.subresourceRange.levelCount = 1;
        //vkCmdPipelineBarrier(context->GetRenderingBuffer(), VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
        imageIndex = imageIndex ? 0 : 1;
    }

    void Map::GetData(Buffer& buffer)
    {
        if(buffer.buffer == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Buffer object uninitialized");
        }
    }

    void Map::CreateVulkanObjects()
    {
        CreatePipelineLayouts();
        CreateBuffers();
        CreateComputePipeline();
        CreateGraphicsPipeline();
    }

    void Map::CreatePipelineLayouts()
    {
        VkDescriptorSetLayout storageLayout = context->device.GetStorageLayout();
        VkDescriptorSetLayout samplerLayout = context->device.GetSamplerLayout();

        VkPipelineLayoutCreateInfo computeLayoutInfo = {};
        computeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        computeLayoutInfo.setLayoutCount = 2;
        VkDescriptorSetLayout layouts[] = {storageLayout, storageLayout};
        computeLayoutInfo.pSetLayouts = layouts;
        computeLayoutInfo.pushConstantRangeCount = 0;
        computeLayoutInfo.pPushConstantRanges = nullptr;
        if(vkCreatePipelineLayout(context->device.device, &computeLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create compute pipeline layout");
        }

        VkPipelineLayoutCreateInfo graphicsLayoutInfo = {};
        graphicsLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        graphicsLayoutInfo.setLayoutCount = 1;
        graphicsLayoutInfo.pSetLayouts = &samplerLayout;
        graphicsLayoutInfo.pushConstantRangeCount = 0;
        graphicsLayoutInfo.pPushConstantRanges = nullptr;
        if(vkCreatePipelineLayout(context->device.device, &graphicsLayoutInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS )
        {
            throw std::runtime_error("Failed to create graphics pipeline layout");
        }
    }

    
    void Map::CreateBuffers()
    {
        std::vector<Map::Vertex> vertices(4);
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        vertices[0] = {{-0.8, -0.8, 0.0},{0.0, 1.0}};
        vertices[1] = {{0.8, -0.8, 0.0}, {1.0, 1.0}};
        vertices[2] = {{0.8, 0.8, 0.0}, {1.0, 0.0}};
        vertices[3] = {{-0.8, 0.8, 0.0}, {0.0, 0.0}};

        vertexBuffer = context->device.CreateBuffer(vertices.data(), indices.size()*sizeof(Map::Vertex), STATIC, GRAPHICS, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        indexBuffer = context->device.CreateBuffer(indices.data(), indices.size()*sizeof(uint32_t), STATIC, GRAPHICS, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }

    void Map::CreateComputePipeline()
    {
        VkPipelineShaderStageCreateInfo computeStage = {};
        computeStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeStage.pName = "main";
        computeStage.module = context->CreateShaderModule("Shaders/comp.spv");

        VkComputePipelineCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        createInfo.stage = computeStage;
        createInfo.layout = computePipelineLayout;
        createInfo.basePipelineIndex = 0;
        createInfo.basePipelineHandle = 0;
        if(vkCreateComputePipelines(context->device.device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &computePipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Map compute pipeline");
        }
        vkDestroyShaderModule(context->device.device, computeStage.module, nullptr);
    }

    void Map::CreateGraphicsPipeline()
    {
        VkPipelineShaderStageCreateInfo vertexStage = {};
        VkPipelineShaderStageCreateInfo fragmentStage = {};
        vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStage.pName = "main";
        vertexStage.module = context->CreateShaderModule("Shaders/vert.spv");
        
        fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStage.pName = "main";
        fragmentStage.module = context->CreateShaderModule("Shaders/frag.spv");

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertexStage, fragmentStage};

        VkPipelineColorBlendAttachmentState attachments = {};
        attachments.blendEnable = VK_FALSE;
        attachments.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.attachmentCount = 1;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.pAttachments = &attachments;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToOneEnable = VK_FALSE;
        multisampling.alphaToCoverageEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterization = {};
        rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.cullMode = VK_CULL_MODE_NONE;
        rasterization.depthBiasEnable = VK_FALSE;
        rasterization.depthClampEnable = VK_FALSE;
        rasterization.depthBiasClamp = 0.0;
        rasterization.depthBiasSlopeFactor = 0.0;
        rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization.lineWidth = 1.0f;
        rasterization.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization.rasterizerDiscardEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.width = context->windowExtent.width;
        viewport.height = context->windowExtent.height;
        viewport.x = 0.0;
        viewport.y = 0.0;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0,0};
        scissor.extent = context->windowExtent;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.pScissors = &scissor;

        VkVertexInputBindingDescription binding = {};
        binding.binding = 0;
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        binding.stride = sizeof(Vertex);

        VkVertexInputAttributeDescription attributes[2];
        attributes[0].binding = 0;
        attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].location = 0;
        attributes[0].offset = 0;

        attributes[1].binding = 0;
        attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[1].location = 1;
        attributes[1].offset = sizeof(glm::vec3);

        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexBindingDescriptionCount = 1;
        vertexInput.vertexAttributeDescriptionCount = 2;
        vertexInput.pVertexBindingDescriptions = &binding;
        vertexInput.pVertexAttributeDescriptions = attributes;

        VkGraphicsPipelineCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.basePipelineHandle = 0;
        createInfo.basePipelineIndex = 0;
        createInfo.layout = graphicsPipelineLayout;
        createInfo.pColorBlendState = &colorBlending;
        createInfo.pInputAssemblyState = &inputAssembly;
        createInfo.pMultisampleState = &multisampling;
        createInfo.subpass = 0;
        createInfo.pRasterizationState = &rasterization;
        createInfo.pStages = shaderStages;
        createInfo.stageCount = 2;
        createInfo.renderPass = context->renderPass;
        createInfo.pViewportState = &viewportState;
        createInfo.pVertexInputState = &vertexInput;

        if(vkCreateGraphicsPipelines(context->device.device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Map's Graphics Pipeline");
        }

        vkDestroyShaderModule(context->device.device, vertexStage.module, nullptr);
        vkDestroyShaderModule(context->device.device, fragmentStage.module, nullptr);
    }

}