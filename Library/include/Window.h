#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Library
{
    class Window
    {
        public:
            static void Init();
            static void Terminate();
            void Create(int width, int height, const char* title);
            void Destroy();
            void PollEvents();
            bool WindowShouldClose();

            int getWidth() const    {return width;}
            int getHeight() const   {return height;}

            GLFWwindow* getWindowPtr() {return window;}

        private:
            GLFWwindow* window;
            int width;
            int height;
    };
}