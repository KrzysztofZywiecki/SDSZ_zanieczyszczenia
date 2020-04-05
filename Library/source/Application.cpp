#include "Application.h"

namespace Library
{
    Application::Application()
    {
        window.Init();
        window.Create(800, 600, "Najlepsze okno");
        context.InitVulkan(&window);
    }

    Application::~Application()
    {
        context.CleanUP();
        window.Destroy();
        window.Terminate();
    }

    void Application::Run()
    {
        while(!window.WindowShouldClose())
        {
            context.DoTheThing();
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

