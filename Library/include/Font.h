#pragma once

#include <glm/glm.hpp>

#include "ft2build.h"
#include FT_FREETYPE_H

#include <unordered_map>

#define FONT_SIZE 32

namespace Library
{

    class Font 
    {
        public:
            Font(const char* fontFile, FT_Library library);
            ~Font();
            
            glm::vec2 GetOffset(char character);
            uint32_t GetIndex(char character);

            unsigned char* GetData(){return data;}
            constexpr uint32_t GetDataSize(){return sheetSize;}

            struct Character
            {
                float advance;
                float y_bearing;
            };
            std::vector<Character> charData;
        private:
            const uint32_t sheetSize = FONT_SIZE * FONT_SIZE * 8 * 8 * 4;
            unsigned char* data;
    };

}