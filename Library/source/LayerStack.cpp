#include "LayerStack.h"

namespace Library
{
    LayerStack::LayerStack()
    {

    }

    LayerStack::~LayerStack()
    {
        for(auto iterator = rbegin(); iterator != rend(); iterator++)
        {
            (*iterator)->onDetach();
        }
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        layer->onAttach();
        layerStack.push_back(layer);
    }

    void LayerStack::PopLayer()
    {
        layerStack.back()->onDetach();
        layerStack.pop_back();
    }
    
}