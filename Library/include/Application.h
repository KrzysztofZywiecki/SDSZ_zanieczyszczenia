#pragma once
#include "LayerStack.h"
#include "Window.h"
#include "Context.h"
#include "Renderer.h"

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
            Renderer renderer;
            LayerStack layerStack;
    };

    Application* CreateApplication();
}