#pragma once

#include "Context.h"

namespace Library
{

    class Map
    {
        public:
            Map(Context* context, uint32_t width, uint32_t height, void* data, VkFormat format, size_t pixelSize);
            ~Map();
            
            void SetDifficultyMap( uint32_t width, uint32_t height, void* data, VkFormat format, size_t pixelSize );
            void SetWind( uint32_t width, uint32_t height, void* data, VkFormat format, size_t pixelSize );
            void SetSimulationProperties(float timeScale, float unitLength);

            void DispatchCompute(float FrameTime);
            void GetData(Buffer& buffer);
            void Render();

			void CleanUp();

            uint32_t GetWidth(){return width;}
            uint32_t GetHeight(){return height;}
            uint32_t GetPixelSize(){return pixelSize;}

        private:
            struct Vertex
            {
                glm::vec3 pos;
                glm::vec2 texCoord;
            };

            struct DispatchInfo
            {
                uint32_t size_x;
                uint32_t size_y;
                float time_scale;
                float unit_length;
            };

            void CreateVulkanObjects();
            void CreateDescriptorSets();
            void CreatePipelineLayouts();
			void CreateBuffers();
            void CreateComputePipeline();
            void CreateGraphicsPipeline();

            VkPipeline graphicsPipeline;
            VkPipeline computePipeline;
            VkPipelineLayout computePipelineLayout;
            VkPipelineLayout graphicsPipelineLayout;

            Context* context;
            Image sources;
            Image images[2];
            Image difficultyMap;
            Image windMap;
            Buffer dispatchInfo;
            VkDescriptorSet dispatchInfoSet;
            VkDescriptorSetLayout dispatchInfoSetLayout;
            VkDescriptorPool descriptorPool;

            uint8_t imageIndex = 0;

            size_t pixelSize;
            uint32_t width;
            uint32_t height;

            Buffer vertexBuffer;
            Buffer indexBuffer;
    };

}
