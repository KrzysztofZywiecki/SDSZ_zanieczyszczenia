#include "Image.h"

namespace Library
{
  
    Image::Image()
		:owner(COMPUTE), image(VK_NULL_HANDLE), imageView(VK_NULL_HANDLE), sampler(VK_NULL_HANDLE), memory(VK_NULL_HANDLE)
    {
        
    }    

}