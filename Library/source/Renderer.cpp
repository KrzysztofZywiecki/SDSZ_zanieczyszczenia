#include "Renderer.h"

namespace Library
{

    void Renderer::SetRenderingBuffer(VkCommandBuffer* commandBuffer)
    {
        renderingCommandBuffer = commandBuffer;
    }

    void Renderer::Render(Drawable& drawable)
    {
        
    }

}