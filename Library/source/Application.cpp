#include "Application.h"

namespace Library
{
    Application::Application()
    {
        window.Init();
        window.Create(800, 600, "Najlepsze okno");
        context.InitFreetype();
        context.InitVulkan(&window);
        renderer.Init(&context);
    }

    Application::~Application()
    {
        vkWaitForFences(context.device.device, SIMULTANEOUS_FRAMES, context.presentationFinishedFences.data(), VK_TRUE, UINT32_MAX);
        renderer.CleanUP();
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
            renderer.Reset();
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
            renderer.Submit();
            context.EndRendering();
            context.EndFrame();
            window.PollEvents();
            std::cout<<(int)(1/glfwGetTime())<<std::endl;
        }
    }

}

