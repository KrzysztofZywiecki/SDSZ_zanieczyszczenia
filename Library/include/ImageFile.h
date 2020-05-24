#pragma once
#include "stb_image.h"

namespace Library
{

    class ImageFile
    {
        public:
            ImageFile(const char* filename);
            ~ImageFile();

            void Close();
            int GetWidth(){return width;}
            int GetHeight(){return height;}
            unsigned char* GetData(){return data;}
        private:
            int width, height;
            unsigned char* data = nullptr;       

    };

}