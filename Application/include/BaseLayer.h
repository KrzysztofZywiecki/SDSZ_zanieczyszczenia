#pragma once

#include "Layer.h"
#include "Map.h"
#include "Font.h"
#include "Rectangle.h"
#include "Renderer.h"
#include "Text.h"

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
        Library::Image fontImage;
        Library::TextureAtlas fontTextureAtlas;
		Library::TextureAtlas atlas;
        Library::TexturedQuad rect[NKWADRATOW];
		Library::Context* context;
        Library::Renderer* renderer;
        Library::Map* map;
        Library::Font* font;
        Library::Text* text;
};