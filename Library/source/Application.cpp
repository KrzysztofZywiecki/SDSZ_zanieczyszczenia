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
            for(auto iterator = layerStack.rbegin(); iterator != layerStack.rend(); iterator++)
            {
                (*iterator)->Update();
            }
            context.BeginFrame();
            context.BeginRendering();
            for(auto layer : layerStack)
            {
                layer->Render();
            }
            context.EndRendering();
            context.EndFrame();
            window.PollEvents();
        }
    }

}

