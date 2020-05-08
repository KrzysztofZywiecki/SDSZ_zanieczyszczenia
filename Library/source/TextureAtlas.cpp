#include "TextureAtlas.h"

namespace Library
{

    uint32_t TextureAtlas::atlasCounter = 0;

    TextureAtlas::TextureAtlas(Image* image, uint32_t x_tiles, uint32_t y_tiles)
        :image(image), x_tiles(x_tiles), y_tiles(y_tiles)
    {
        scale = glm::vec2({1/(float)x_tiles, 1/(float)y_tiles});
        index = atlasCounter;
        atlasCounter++;
    }

	TextureAtlas::TextureAtlas()
	{

	}

    VkDescriptorSet TextureAtlas::GetSamplerSet()
    {
        return image->samplerBinding;
    }

    glm::vec2 TextureAtlas::GetOffset(uint32_t x, uint32_t y)
    {
        glm::vec2 offset({(float)x*scale.x, (float)y*scale.y});
        return offset;
    }

    bool TextureAtlas::operator<(TextureAtlas& rhs)
    {
        return index < rhs.index;
    }

}