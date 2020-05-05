#pragma once

#include "Layer.h"
#include "Map.h"
#include "Rectangle.h"
#include "Renderer.h"

#define NKWADRATOW 150

class BaseLayer : public Library::Layer 
{
    public:
	BaseLayer(Library::Context* context, Library::Renderer* renderer);
    void onAttach();
    void onDetach();

    void Update();
    void Render();

    private:
        Library::Rectangle rect[NKWADRATOW];
		Library::Context* context;
        Library::Renderer* renderer;
        Library::Map* map;
};