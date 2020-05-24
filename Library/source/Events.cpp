#include "Events.h"


namespace Library
{

    Window* Events::window;
    double Events::absoluteTime;
    double Events::previousTime = 0;


    void Events::Init(Window* window)
    {
        Events::window = window;
        absoluteTime = glfwGetTime();
    }

    void Events::PollEvents()
    {
        glfwPollEvents();
        double time = glfwGetTime();
        previousTime = time - absoluteTime;
        absoluteTime = time;
    }

    bool Events::WindowShouldClose()
    {
        return glfwWindowShouldClose(window->getWindowPtr());
    }

    glm::vec2 Events::GetNormalizedMousePosition()
    {
        double xPosition, yPosition;
        glfwGetCursorPos(window->getWindowPtr(), &xPosition, &yPosition);
        
        xPosition = xPosition / static_cast<double>(window->getWidth()) * 2.0 - 1.0;
        yPosition = yPosition / static_cast<double>(window->getHeight()) * -2.0 + 1.0;

        return glm::vec2(xPosition, yPosition);
    }


}