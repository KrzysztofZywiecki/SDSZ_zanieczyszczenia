#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Library
{
    class Window
    {
        public:
            static void Init();
            static void Terminate();
            void Create(uint32_t width, uint32_t height, const char* title);
            void Destroy();
            void PollEvents();
            bool WindowShouldClose();

            uint32_t getWidth() const    {return width;}
            uint32_t getHeight() const   {return height;}

            GLFWwindow* getWindowPtr() {return window;}

        private:
            GLFWwindow* window;
            uint32_t width;
            uint32_t height;
    };
}