#pragma once
#include "LayerStack.h"

namespace Library
{
    class Application
    {
        public:
            Application();
            ~Application();

            void Run();
        protected:
            LayerStack layerStack;
    };

    Application* CreateApplication();
}