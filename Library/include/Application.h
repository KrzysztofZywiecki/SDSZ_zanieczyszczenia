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
            Context context;
            LayerStack layerStack;
    };

    Application* CreateApplication();
}