#pragma once
#include "LayerStack.h"
#include "Window.h"
#include "Context.h"
#include "Renderer.h"
#include "Events.h"

namespace Library
{
    class Application
    {
        public:
            Application();
            ~Application();

            void Run();
        protected:
            float frameTime = 0;
            Window window;
            Context context;
            Renderer renderer;
            LayerStack layerStack;
    };

    Application* CreateApplication();
}