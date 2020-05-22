#include "Text.h"

namespace Library
{

    Text::Text(Font* font, TextureAtlas* fontBitmap, Renderer* renderer, const char* text, glm::vec2 origin, float scale)
        : font(font), atlas(fontBitmap), renderer(renderer), origin(origin), scale(scale)
    {
        glm::vec2 offset = origin;
        
        for(uint32_t i = 0; i < strlen(text); i++)
        {
            if(text[i] == ' ')
            {
                offset.x += scale/2;
            }
            else if(text[i] == '\n')
            {
                offset.x = origin.x;
                offset.y += scale;
            }
            else
            {
                uint32_t index = font->GetIndex(text[i]);
                TexturedQuad quad;
                quad.UseTexture(atlas);
                quad.SetPosition({offset.x, offset.y - (font->charData[index].y_bearing - FONT_SIZE)/FONT_SIZE * scale, 0.0});
                quad.SetScale({scale, scale, 1.0});
                quad.SetRotation(0.0f);
                quad.UseTile(index % 8, index / 8);
                letters.push_back(quad);
                offset.x += (font->charData[index].advance/(float)FONT_SIZE + 0.1) * scale;
            }
        }
    }

    void Text::Render()
    {
        for(auto quad : letters)
        {
            renderer->Render(quad);
        }
    }

}