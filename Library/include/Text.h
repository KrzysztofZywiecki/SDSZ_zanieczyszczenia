#pragma once

#include "Renderer.h"
#include "TexturedQuad.h"
#include "Font.h"
#include "TextureAtlas.h"
#include <vector>

namespace Library
{

    class Text
    {
        public:
            Text(Font* font, TextureAtlas* fontBitmap, Renderer* renderer, const char* text, glm::vec2 origin, float scale);
            
            void Render();
            void UpdateText(std::string text);
            static std::string FloatToString();

        private:
            Font* font;
            TextureAtlas* atlas;
            Renderer* renderer;
            glm::vec2 origin;
            float scale;
            std::vector<TexturedQuad> letters;
    };

}