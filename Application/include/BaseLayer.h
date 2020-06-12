#pragma once

#include "Layer.h"
#include "Map.h"
#include "Font.h"
#include "Rectangle.h"
#include "Renderer.h"
#include "Text.h"
#include "Events.h"
#include "ImageFile.h"
#include "ConfigFile.h"

#define NKWADRATOW 60

enum State
{
    PAUSED,
    SIMULATING
};

class BaseLayer : public Library::Layer 
{
    public:
	BaseLayer(Library::Context* context, Library::Renderer* renderer);
    void onAttach();
    void onDetach();

    void Update(float frameTime);
    void Render();

    private:
        const float diffusionSamples = 50.0f;
        const float windSamples = 1.0f;
        const uint32_t size = 1024;

        uint32_t sampleNumber = 0;
        float samplesPS = 0;
        ConfigFile* conf;
        State state = SIMULATING;
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