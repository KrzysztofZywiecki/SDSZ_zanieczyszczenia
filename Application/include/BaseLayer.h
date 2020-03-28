#pragma once

#include "Layer.h"

class BaseLayer : public Library::Layer 
{
    public:
    void onAttach();
    void onDetach();

    void Update();
    void Render();

    private:

};