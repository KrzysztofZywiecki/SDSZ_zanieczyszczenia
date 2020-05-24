#pragma once

#include "Window.h"
#include <glm/glm.hpp>

namespace Library
{

    class Events
    {
        public:
            static void Init(Window* windw);
            static void PollEvents();
            static bool WindowShouldClose();
            static glm::vec2 GetNormalizedMousePosition();
            static bool MouseClicked();
            static double GetFrameTime(){return previousTime;};
            static bool KeyPressed(uint32_t key);
            

        private:
            Events();
            static double absoluteTime;
            static double previousTime;
            static Window* window;


    };

}