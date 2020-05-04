#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Drawable.h"

namespace Library
{

    class Renderer
    {
        public:
            void SetRenderingBuffer(VkCommandBuffer* renderingCommandBuffer);
            void Render(Drawable& drawable);
        private:
            VkCommandBuffer* renderingCommandBuffer;

    };

}