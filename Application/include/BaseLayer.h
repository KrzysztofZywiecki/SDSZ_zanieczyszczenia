#pragma once

#include "Layer.h"
#include "Map.h"
#include "Rectangle.h"
#include "Renderer.h"

#define NKWADRATOW 60

class BaseLayer : public Library::Layer 
{
    public:
	BaseLayer(Library::Context* context, Library::Renderer* renderer);
    void onAttach();
    void onDetach();

    void Update(float frameTime);
    void Render();

    private:
		Library::Image texture;
		Library::TextureAtlas atlas;
        Library::TexturedQuad rect[NKWADRATOW];
		Library::Context* context;
        Library::Renderer* renderer;
        Library::Map* map;
};