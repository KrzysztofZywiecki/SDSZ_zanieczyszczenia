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

    void Window::Create(int width, int height, const char* title)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    }

    void Window::Destroy()
    {
        glfwDestroyWindow(window);
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    bool Window::WindowShouldClose()
    {
        return glfwWindowShouldClose(window);
    }
}