#pragma once
#include "LayerStack.h"
#include "Window.h"
#include "Context.h"

namespace Library
{
    class Application
    {
        public:
            Application();
            ~Application();

            void Run();
        protected:
            Window window;
            LayerStack layerStack;
    };

    Application* CreateApplication();
}