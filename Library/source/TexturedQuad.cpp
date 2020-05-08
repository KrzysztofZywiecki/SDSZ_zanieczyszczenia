#include "TexturedQuad.h"

namespace Library
{
    TexturedQuad::TexturedQuad(TextureAtlas* texture, uint32_t x_tile, uint32_t y_tile)
        : texture(texture), tile_x(tile_x), tile_y(tile_y)
    {    
    }

    TexturedQuad::TexturedQuad()
    {
    }

    TexturedQuad::Transform TexturedQuad::ApplyTransforms_t()
    {
        Transform transform;
        transform.matrixTransform = ApplyTransforms();
        transform.textureScale = texture->GetScale();
        transform.textureOffset = texture->GetOffset(tile_x, tile_y);
        return transform;
    }

    void TexturedQuad::UseTexture(TextureAtlas* texture)
    {
        this->texture = texture;
    }

    void TexturedQuad::UseTile(uint32_t x, uint32_t y)
    {
        tile_x = x;
        tile_y = y;
    }

}