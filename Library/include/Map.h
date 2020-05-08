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
            struct Vertex
            {
                glm::vec3 pos;
                glm::vec2 texCoord;
            };
            void CreateVulkanObjects();
            void CreatePipelineLayouts();
			void CreateBuffers();
            void CreateComputePipeline();
            void CreateGraphicsPipeline();

            VkPipeline graphicsPipeline;
            VkPipeline computePipeline;
            VkPipelineLayout computePipelineLayout;
            VkPipelineLayout graphicsPipelineLayout;

            Context* context;
            Image images[2];

            uint8_t imageIndex = 0;

            size_t pixelSize;
            uint32_t width;
            uint32_t height;

            Buffer vertexBuffer;
            Buffer indexBuffer;
    };

}
