#pragma once

#include "Context.h"

namespace Library
{

    class Map
    {
        public:
            Map(Context* context, uint32_t width, uint32_t height, void* data, VkFormat format, size_t pixelSize);
            ~Map();

            void DispatchCompute(bool acquireData);
            void GetData(Buffer& buffer);
            void Render();

			void CleanUp();

            uint32_t GetWidth(){return width;}
            uint32_t GetHeight(){return height;}
            uint32_t GetPixelSize(){return pixelSize;}
        private:
            void CreateVulkanObjects();
            void CreateDescriptorSetLayouts();
            void CreatePipelineLayouts();
            void CreateDescriptorSets();
			void CreateBuffers();
            void CreateComputePipeline();
            void CreateGraphicsPipeline();

            VkPipeline graphicsPipeline;
            VkPipeline computePipeline;
            VkPipelineLayout computePipelineLayout;
            VkPipelineLayout graphicsPipelineLayout;
            Context* context;
            Image images[2];

            uint8_t imageIndex;

            size_t pixelSize;
            uint32_t width;
            uint32_t height;

            VkDescriptorPool descriptorPool;
            VkDescriptorSetLayout storageLayout;
            VkDescriptorSetLayout sampledImageLayout;
            VkDescriptorSet storageSets[2];
            VkDescriptorSet sampledImage[2];

            Buffer vertexBuffer;
            Buffer indexBuffer;
    };

}
