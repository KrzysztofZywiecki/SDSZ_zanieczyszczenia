#include "BaseLayer.h"

#include <iostream>
#include <ctime>
#include <random>

std::vector<float> data;

BaseLayer::BaseLayer(Library::Context* context, Library::Renderer* renderer)
	:context(context), renderer(renderer)
{}

void BaseLayer::onAttach()
{
    int size = 256;     //Rozdzielczosc obrazka do obliczen

    std::cout<<"Attached!"<<std::endl;
    data.resize(size*size);

    srand(time(nullptr));
    memset(data.data(), 0, size*size*sizeof(float));
    for(int i = 0; i < 10; i++)
    {
        int x = rand()%size;
        int y = rand()%size;
        
        data[x*size + y] = 5;
    }
	unsigned char tex[] = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 255, 255};
	texture = context->device.CreateImage(VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT,
        2, 2, Library::GRAPHICS, tex, 4*sizeof(unsigned char));
	atlas = Library::TextureAtlas(&context->font, 1, 1);

    map = new Library::Map(context, size, size, data.data(), VK_FORMAT_R32_SFLOAT, sizeof(float));
    float difficulty[] = {0.0f};
    float wind[] = {0.5f, -0.5f, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5};
    map->SetDifficultyMap(1, 1, difficulty, VK_FORMAT_R32_SFLOAT, sizeof(float));
    map->SetWind(2, 2, wind, VK_FORMAT_R32G32_SFLOAT, 2*sizeof(float));
    map->SetSimulationProperties(0.04f, 1.0f);

    font = new Library::Font("Resources/Roboto-Regular.ttf", context->GetFreetype());
    fontImage = context->device.CreateImage(VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT, 8*FONT_SIZE, 8*FONT_SIZE, Library::GRAPHICS, font->GetData(), 4*sizeof(unsigned char));
    fontTextureAtlas = Library::TextureAtlas(&fontImage, 8, 8);
    
    text = new Library::Text(font, &fontTextureAtlas, renderer, "Czytelny fragment\ntekstu w programie", {-0.9, -0.9}, 0.1);

    for(int i = 0; i < NKWADRATOW; i++)
    {
		rect[i].UseTexture(&atlas);
		rect[i].UseTile(0, 0);
        rect[i].SetPosition({(double)rand()/RAND_MAX * 2 - 1, (double)rand()/RAND_MAX * 2 - 1, 0});
        rect[i].SetScale({0.1, 0.1, 1.0});
    }

}

//Jakieś magiczne parametry
//Realizm
//Restart symulacji

void BaseLayer::onDetach()
{
    delete font;
	map->CleanUp();
    context->device.DestroyImage(texture);
    context->device.DestroyImage(fontImage);
}

void BaseLayer::Update(float frameTime)
{
    map->DispatchCompute(frameTime);
}

void BaseLayer::Render()
{
    map->Render();
    text->Render();
}