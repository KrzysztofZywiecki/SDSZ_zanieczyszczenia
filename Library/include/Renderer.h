#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Context.h"

#include "TexturedQuad.h"
#include "Rectangle.h"
#include <map>
#include <vector>

#define MAX_ENTITIES 100

namespace Library
{

    class Renderer
    {
        public:
            void Render(TexturedQuad& quad);
        
            void Init(Context* context);
            void Reset();
            void Submit();
            void CleanUP();
        private:
            struct Unit
            {
                Buffer vertexBuffer;
                Buffer indexBuffer;
                uint32_t indexCount;

                std::vector<TexturedQuad::Transform> transforms;
                std::vector<Buffer> buffers;

                void FillBuffers(Context* context);
                void Reset();
                void Clear(Context* context);
                TextureAtlas* texture;
            };

            Unit rectangles;

            void CreateVulkanObjects();

            void CreateGraphicsPipelineLayout();
            void CreateGraphicsPipeline();

            Context* context;
            VkPipeline graphicsPipeline;
            VkPipelineLayout graphicsPipelineLayout;

            VkCommandBuffer* renderingCommandBuffer;

    };

}