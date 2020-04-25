#pragma once
#include "Layer.h"
#include <vector>

namespace Library
{

    class LayerStack
    {
        public:
            LayerStack();
            ~LayerStack();

            void PushLayer(Layer* layer);
			void Clear();
            void PopLayer();

            std::vector<Layer*>::iterator begin()           {return layerStack.begin();}
            std::vector<Layer*>::iterator end()             {return layerStack.end();}
            std::vector<Layer*>::reverse_iterator rbegin()  {return layerStack.rbegin();}
            std::vector<Layer*>::reverse_iterator rend()    {return layerStack.rend();}
        private:
        std::vector<Library::Layer*> layerStack;
    };

}