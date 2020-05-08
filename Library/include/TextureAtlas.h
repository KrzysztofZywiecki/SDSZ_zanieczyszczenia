#pragma once
#include "glm/glm.hpp"
#include "Image.h"

namespace Library
{

    class TextureAtlas
    {
        public:
            TextureAtlas(Image* image, uint32_t x_tiles, uint32_t y_tiles);
			TextureAtlas();

            VkDescriptorSet GetSamplerSet();
            glm::vec2 GetScale(){return scale;};
            glm::vec2 GetOffset(uint32_t x, uint32_t y);
            uint32_t GetIndex(){return index;};
            bool operator<(TextureAtlas& rhs);
        private:
            Image* image;
            uint32_t x_tiles;
            uint32_t y_tiles;
            uint32_t index;
            glm::vec2 scale;
            static uint32_t atlasCounter;
    };

}