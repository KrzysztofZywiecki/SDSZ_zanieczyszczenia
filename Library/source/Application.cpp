#include "Application.h"

namespace Library
{
    Application::Application()
    {
        window.Init();
        window.Create(800, 600, "Najlepsze okno");
        Context::InitVulkan(window.getWindowPtr());
    }

    Application::~Application()
    {
        Context::CleanUP();
        window.Destroy();
        window.Terminate();
    }

    void Application::Run()
    {
        while(!window.WindowShouldClose())
        {
            for(auto iterator = layerStack.rbegin(); iterator != layerStack.rend(); iterator++)
            {
                (*iterator)->Update();
            }
            for(auto layer : layerStack)
            {
                layer->Render();
            }
            window.PollEvents();
            _sleep(500);
        }
    }

}

