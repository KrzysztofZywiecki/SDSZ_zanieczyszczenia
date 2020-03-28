#include "Application.h"

namespace Library
{
    Application::Application()
    {

    }

    Application::~Application()
    {

    }

    void Application::Run()
    {
        while(true)
        {
            for(auto iterator = layerStack.rbegin(); iterator != layerStack.rend(); iterator++)
            {
                (*iterator)->Update();
            }
            for(auto layer : layerStack)
            {
                layer->Render();
            }
            _sleep(100);
        }
    }

}

