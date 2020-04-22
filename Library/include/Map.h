#pragma once

#include "Device.h"

namespace Library
{

    class Map
    {
        public:
            void DispatchCompute(bool acquireData);
            void Render();
            void SetSize();

            uint32_t GetWidth(){return width;}
            uint32_t GetHeight(){return height;}

            void* MapActiveImage();
            void UnmapActiveImage();

        private:
            VkPipeline graphicsPipeline;
            VkPipeline computePipeline;
            Device* device;
            Image images[2];
            uint32_t width;
            uint32_t height;

            VkSemaphore computeFinishedSemaphore;
            VkSemaphore renderFinishedSemaphore;
            VkFence computeFinishedFence;

    };

}
