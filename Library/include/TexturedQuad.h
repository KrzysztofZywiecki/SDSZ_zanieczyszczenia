#pragma once
#include "Drawable.h"
#include "TextureAtlas.h"

namespace Library
{

    class TexturedQuad : public Drawable
    {
        public:
            struct Transform
            {
                glm::mat4 matrixTransform;
                glm::vec2 textureScale;
                glm::vec2 textureOffset;
            };

            TexturedQuad(TextureAtlas* textureAtlas, uint32_t tile_x, uint32_t tile_y);
            TexturedQuad();

            void UseTile(uint32_t x, uint32_t y);
            void UseTexture(TextureAtlas* texture);

            uint32_t GetTextureIndex(){return texture->GetIndex();};
			TextureAtlas* GetTexturePtr() { return texture; };

            Transform ApplyTransforms_t();
        private:
            TextureAtlas* texture;
            uint32_t tile_x;
            uint32_t tile_y;
    };

}

