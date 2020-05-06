#include "Renderer.h"

namespace Library
{

    void Renderer::Init(Context* context)
    {
        this->context = context;
        CreateVulkanObjects();

        std::vector<Model::Vertex> vertices = { {glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)},
                                            {glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)},
                                            {glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)},
                                            {glm::vec3(-0.5, 0.5, 0.0), glm::vec2(0.0, 1.0)} };
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        rectangles.vertexBuffer = context->device.CreateBuffer(vertices.data(), 4*sizeof(Model::Vertex), STATIC, GRAPHICS, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        rectangles.indexBuffer = context->device.CreateBuffer(indices.data(), 6*sizeof(uint32_t), STATIC, GRAPHICS, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        rectangles.indexCount = 6;
    }

    void Renderer::Reset()
    {
        rectangles.Reset();
    }

    void Renderer::CleanUP()
    {
        rectangles.Clear(context);
        vkDestroyPipelineLayout(context->device.device, graphicsPipelineLayout, nullptr);
        vkDestroyPipeline(context->device.device, graphicsPipeline, nullptr);
    }

    void Renderer::Render(Rectangle& rect)
    {
        rectangles.transforms.push_back(rect.ApplyTransforms());
    }

    void Renderer::Submit()
    {
        if(rectangles.transforms.size() == 0)
        {
            return;
        }
        VkCommandBuffer commandBuffer = context->GetRenderingBuffer();

		rectangles.FillBuffers(context);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        vkCmdBindIndexBuffer(commandBuffer, rectangles.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
        
        int batches = rectangles.transforms.size() / MAX_ENTITIES + (rectangles.transforms.size() % MAX_ENTITIES ? 0 : 1);
        int lastBatchCount = rectangles.transforms.size() % MAX_ENTITIES;
        for(int i = 0; i < batches; i++)
        {
            VkBuffer vertexBuffers[] = {rectangles.vertexBuffer.buffer, rectangles.buffers[i].buffer};
            VkDeviceSize offsets[] = {0,0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
            vkCmdDrawIndexed(commandBuffer, rectangles.indexCount, MAX_ENTITIES, 0, 0, 0);
        }
        if(lastBatchCount != 0)
        {
            VkBuffer vertexBuffers[] = {rectangles.vertexBuffer.buffer, rectangles.buffers[batches].buffer};
            VkDeviceSize offsets[] = {0,0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
            vkCmdDrawIndexed(commandBuffer, rectangles.indexCount, lastBatchCount, 0, 0, 0);
        }
    }

    void Renderer::RenderUnit::FillBuffers(Context* context)
    {
        int buffersToCreate = (transforms.size() / MAX_ENTITIES + 1) - buffers.size();
        size_t dataSize = MAX_ENTITIES * sizeof(glm::mat4);

        for(int i = 0; i < buffersToCreate; i++)
        {
            buffers.push_back(context->device.CreateBuffer(nullptr, MAX_ENTITIES*sizeof(glm::mat4), DYNAMIC, GRAPHICS, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
        }

        for(int i = 0; i < buffers.size() - 1; i++)
        {
            void* data;
            vkMapMemory(context->device.device, buffers[i].memory, 0, dataSize, 0, &data);
            memcpy(data, transforms.data() + i * dataSize, dataSize);
            vkUnmapMemory(context->device.device, buffers[i].memory);
        }

        size_t endSize = (transforms.size() % MAX_ENTITIES) * sizeof(glm::mat4);
        void* data;
        vkMapMemory(context->device.device, buffers[buffers.size()-1].memory, 0, endSize, 0, &data);
        memcpy(data, transforms.data() + (buffers.size()-1)*MAX_ENTITIES, endSize); 
        vkUnmapMemory(context->device.device, buffers[buffers.size()-1].memory);
    }

    void Renderer::RenderUnit::Reset()
    {
        transforms.clear();
    }

    void Renderer::RenderUnit::Clear(Context* context)
    {
        for(Buffer buffer : buffers)
        {
            context->device.DestroyBuffer(buffer);
        }
        context->device.DestroyBuffer(vertexBuffer);
        context->device.DestroyBuffer(indexBuffer);
    }

    void Renderer::CreateVulkanObjects()
    {
        CreateGraphicsPipelineLayout();
        CreateGraphicsPipeline();
    }

    void Renderer::CreateGraphicsPipelineLayout()
    {
        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;
        layoutInfo.setLayoutCount = 0;
        layoutInfo.pSetLayouts = nullptr;

        if(vkCreatePipelineLayout(context->device.device, &layoutInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline layout");
        }
    }

    void Renderer::CreateGraphicsPipeline()
    {

        VkPipelineShaderStageCreateInfo vertexStage = {};
        vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStage.module = context->CreateShaderModule("Shaders/rVert.spv");
        vertexStage.pName = "main";
        vertexStage.pSpecializationInfo = nullptr;
        vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        VkPipelineShaderStageCreateInfo fragmentStage = {};
        fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStage.module = context->CreateShaderModule("Shaders/rFrag.spv");
        fragmentStage.pName = "main";
        fragmentStage.pSpecializationInfo = nullptr;
        fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkPipelineShaderStageCreateInfo stages[] = {vertexStage, fragmentStage};

        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 0;
        dynamicState.pDynamicStates = nullptr;

        VkPipelineColorBlendAttachmentState attachment = {};
        attachment.blendEnable = VK_FALSE;
        attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        
        VkPipelineColorBlendStateCreateInfo colorBlendState = {};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.attachmentCount = 1;
        colorBlendState.blendConstants[0] = 0.0f;
        colorBlendState.blendConstants[1] = 0.0f;
        colorBlendState.blendConstants[2] = 0.0f;
        colorBlendState.blendConstants[3] = 0.0f;
        colorBlendState.logicOpEnable = VK_FALSE;
        colorBlendState.logicOp = VK_LOGIC_OP_SET;
        colorBlendState.pAttachments = &attachment;

        VkRect2D scissor = {};
        scissor.extent = context->windowExtent;
        scissor.offset = {0,0};

        VkViewport viewport = {};
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = context->windowExtent.width;
        viewport.height = context->windowExtent.height;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        viewportState.pViewports = &viewport;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyState.primitiveRestartEnable = VK_FALSE;

        VkVertexInputBindingDescription bindings[2];
        bindings[0].binding = 0;
        bindings[0].stride = sizeof(Model::Vertex);
        bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindings[1].binding = 1;
        bindings[1].stride = sizeof(glm::mat4);
        bindings[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        VkVertexInputAttributeDescription attributes[6];
        attributes[0].binding = 0;
        attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].location = 0;
        attributes[0].offset = 0;
        attributes[1].binding = 0;
        attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[1].location = 1;
        attributes[1].offset = sizeof(glm::vec3);

        attributes[2].binding = 1;
        attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributes[2].location = 2;
        attributes[2].offset = 0;
        attributes[3].binding = 1;
        attributes[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributes[3].location = 3;
        attributes[3].offset = sizeof(glm::vec4);
        attributes[4].binding = 1;
        attributes[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributes[4].location = 4;
        attributes[4].offset = 2 * sizeof(glm::vec4);;
        attributes[5].binding = 1;
        attributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributes[5].location = 5;
        attributes[5].offset = 3 * sizeof(glm::vec4);;

        VkPipelineVertexInputStateCreateInfo vertexInputState = {};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.vertexBindingDescriptionCount = 2;
        vertexInputState.vertexAttributeDescriptionCount = 6;
        vertexInputState.pVertexBindingDescriptions = bindings;
        vertexInputState.pVertexAttributeDescriptions = attributes;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.pSampleMask = nullptr;
        multisampling.minSampleShading = 1.0f;
        multisampling.alphaToOneEnable = VK_FALSE;
        multisampling.alphaToCoverageEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterization = {};
        rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.cullMode = VK_CULL_MODE_NONE;
        rasterization.depthBiasClamp = 0.0f;
        rasterization.depthBiasConstantFactor = 0.0f;
        rasterization.depthBiasEnable = VK_FALSE;
        rasterization.depthBiasSlopeFactor = 0.0f;
        rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization.lineWidth = 1.0f;
        rasterization.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization.rasterizerDiscardEnable = VK_FALSE;

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;       
        pipelineInfo.layout = graphicsPipelineLayout;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = stages;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.renderPass = context->renderPass;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlendState;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineInfo.pVertexInputState = &vertexInputState;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pRasterizationState = &rasterization;

        if(vkCreateGraphicsPipelines(context->device.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline");
        }
        vkDestroyShaderModule(context->device.device, vertexStage.module, nullptr);
        vkDestroyShaderModule(context->device.device, fragmentStage.module, nullptr);
    }

}