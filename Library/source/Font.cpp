#include "Font.h"


namespace Library
{

    Font::Font(const char* fontFile, FT_Library library)
    {
        FT_Face face;
        FT_New_Face(library, fontFile, 0, &face);
        FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);
        char characters[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-"};
    
        data = new unsigned char[sheetSize];
        memset(data, 255, sheetSize);

        for(uint32_t i = 0; i < 64; i++)
        {
            char character = characters[i];
            FT_Load_Char(face, character, FT_LOAD_RENDER);

            uint32_t big_x = i % 8;
            uint32_t big_y = i / 8;

            for(uint32_t y = 0; y < FONT_SIZE; y++)
            {
                for(uint32_t x = 0; x < FONT_SIZE; x++)
                {
                    if(x < face->glyph->bitmap.width && y < face->glyph->bitmap.rows)
                    {
                        data[big_x * FONT_SIZE * 4 + big_y * FONT_SIZE * FONT_SIZE * 8 * 4 + x * 4 + FONT_SIZE * 8 * y * 4 + 3] =
                            face->glyph->bitmap.buffer[x + face->glyph->bitmap.width * y];
                    }
                    else
                    {
                        data[big_x * FONT_SIZE * 4 + big_y * FONT_SIZE * FONT_SIZE * 8 * 4 + x * 4 + FONT_SIZE * 8 * y * 4 + 3] = 0;
                    }
                }
            }
            charData.push_back({(float)(face->glyph->bitmap.width), (float)(face->glyph->bitmap_top)});
        }
        FT_Done_Face(face);
    }

    Font::~Font()
    {
        delete[] data;
    }

    uint32_t Font::GetIndex(char character)
    {
        if(character >= 'a' && character <= 'z')
        {
            return character - 'a';
        }
        else if(character >= 'A' && character <= 'Z')
        {
            return character - 'A' + 26;
        }
        else if(character >= '0' && character <= '9')
        {
            return character - '0' + 52;
        }
        else if (character == '.')
        {
            return 62;
        }
        else if(character == '-')
        {
            return 63;
        }
        else
        {
            return 0;
        }
    }

}