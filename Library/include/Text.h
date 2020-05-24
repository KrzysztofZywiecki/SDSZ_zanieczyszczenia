#pragma once

#include "Renderer.h"
#include "TexturedQuad.h"
#include "Font.h"
#include "TextureAtlas.h"
#include <string>
#include <vector>

namespace Library
{

    class Text
    {
        public:
            Text(Font* font, TextureAtlas* fontBitmap, Renderer* renderer, const char* text, glm::vec2 origin, float scale);
            
            void Render();
            void UpdateText(std::string text);
            static std::string FloatToString(float number, uint32_t precision);

        private:
            void PutLetter(char character);
            Font* font;
            TextureAtlas* atlas;
            Renderer* renderer;
            glm::vec2 origin;
            glm::vec2 offset;
            float scale;
            uint32_t textLen;
            std::vector<TexturedQuad> letters;
    };

}