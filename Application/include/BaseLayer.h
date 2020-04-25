#pragma once

#include "Layer.h"
#include "Map.h"

class BaseLayer : public Library::Layer 
{
    public:
	BaseLayer(Library::Context* context);
    void onAttach();
    void onDetach();

    void Update();
    void Render();

    private:
		Library::Context* context;
        Library::Map* map;
};