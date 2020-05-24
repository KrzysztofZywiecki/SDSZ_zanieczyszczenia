#include "Window.h"
#include <stdexcept>

namespace Library
{
    void Window::Init()
    {
        int result = glfwInit();
        if(result == -1)
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }
    }

    void Window::Terminate()
    {
        glfwTerminate();
    }

    void Window::Create(uint32_t width, uint32_t height, const char* title)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        this->width = width;
        this->height = height;
    }

    void Window::Destroy()
    {
        glfwDestroyWindow(window);
    }
}