#include "Text.h"

namespace Library
{

    Text::Text(Font* font, TextureAtlas* fontBitmap, Renderer* renderer, const char* text, glm::vec2 origin, float scale)
        : font(font), atlas(fontBitmap), renderer(renderer), origin(origin), scale(scale)
    {
        letters.reserve(strlen(text));
        offset = origin;
		textLen = 0;
        for(uint32_t i = 0; i < strlen(text); i++)
        {
            PutLetter(text[i]);
        }
    }

    void Text::UpdateText(std::string text)
    {
        letters.clear();
        offset = origin;
		textLen = 0;
        if(text.length() < letters.size())
        {
            letters.reserve(text.length());
        }
        for(uint32_t i = 0; i < text.length(); i++)
        {
            PutLetter(text[i]);
        }
    }

    void Text::PutLetter(char character)
    {
        if(character == ' ')
        {
            offset.x += scale/2;
        }
        else if(character == '\n')
        {
            offset.x = origin.x;
            offset.y -= scale;
        }
        else
        {
            uint32_t index = font->GetIndex(character);
            TexturedQuad quad;
            quad.UseTexture(atlas);
            quad.SetPosition({offset.x, offset.y + (font->charData[index].y_bearing - FONT_SIZE)/FONT_SIZE * scale, 0.0});
            quad.SetScale({scale, scale, 1.0});
            quad.SetRotation(0.0f);
            quad.UseTile(index % 8, index / 8);
            letters.push_back(quad);
            offset.x += (font->charData[index].advance/(float)FONT_SIZE + 0.1) * scale;
			textLen++;
        }
    }

    std::string Text::FloatToString(float number, uint32_t precision)
    {
        std::string ret;
        if(number < 0)
        {
            ret.push_back('-');
			number *= -1.0;
        }
        int wholePart = static_cast<int>(number);
        if(wholePart == 0)
        {
            ret.push_back('0');
        }
        else
        {
            std::string wh_flipped;
            while(wholePart != 0)
            {
                wh_flipped.push_back(wholePart%10 + '0');
				wholePart /= 10;
            }
            ret.append(wh_flipped.rbegin(), wh_flipped.rend());
        }

        if(precision > 0)
        {
			ret.push_back('.');
            float prec = 10.0;
            for(uint32_t i = 0; i < precision; i++)
            {
                ret.push_back(int(number * prec) % 10  + '0');
				prec *= 10.0;
            }
        }
        return ret;
    }

    void Text::Render()
    {
        for(uint32_t i = 0; i < textLen; i++)
        {
            renderer->Render(letters[i]);
        }
    }

}