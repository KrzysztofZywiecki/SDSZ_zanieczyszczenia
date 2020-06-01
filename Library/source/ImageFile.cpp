#include "ImageFile.h"

namespace Library
{

    ImageFile::ImageFile(const char* filename)
    {
        int n;
        data = stbi_load(filename, &width, &height, &n, 4);
    }

    ImageFile::~ImageFile()
    {
        Close();
    }

    void ImageFile::Close()
    {
        if(data != nullptr)
        {
			data = nullptr;
            stbi_image_free(data);
        }
    }
}