#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Context.h"

#include "Rectangle.h"
#include <map>
#include <vector>

#define MAX_ENTITIES 100

namespace Library
{

    class Renderer
    {
        public:
            void Render(Rectangle& drawable);
        
            void Init(Context* context);
            void Reset();
            void Submit();
            void CleanUP();
        private:
            struct RenderUnit
            {
                Buffer vertexBuffer;
                Buffer indexBuffer;
                uint32_t indexCount;

                std::vector<glm::mat4> transforms;
                std::vector<Buffer> buffers;

                void FillBuffers(Context* context);
                void Reset();
                void Clear(Context* context);
            };

            void CreateVulkanObjects();

            void CreateGraphicsPipelineLayout();
            void CreateGraphicsPipeline();

            Context* context;
            VkPipeline graphicsPipeline;
            VkPipelineLayout graphicsPipelineLayout;

            RenderUnit rectangles;
            VkCommandBuffer* renderingCommandBuffer;

    };

}