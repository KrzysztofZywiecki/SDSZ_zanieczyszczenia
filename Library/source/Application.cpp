#include "Application.h"

namespace Library
{
    Application::Application()
    {
        window.Init();
        window.Create(800, 600, "Najlepsze okno");
        context.InitFreetype();
        context.InitVulkan(&window);
    }

    Application::~Application()
    {
		layerStack.Clear();
        context.CleanUP();
        window.Destroy();
        window.Terminate();
    }

    void Application::Run()
    {
        while(!window.WindowShouldClose())
        {
            glfwSetTime(0);
            context.BeginFrame();
            context.BeginRendering();
            
            context.BeginComputeOperations();
            for(auto iterator = layerStack.rbegin(); iterator != layerStack.rend(); iterator++)
            {
                (*iterator)->Update();
            }
            context.EndComputeOperations();
            
            for(auto layer : layerStack)
            {
                layer->Render();
            }
            context.EndRendering();
            context.EndFrame();
            window.PollEvents();
            std::cout<<(int)(1/glfwGetTime())<<std::endl;
        }
    }

}

