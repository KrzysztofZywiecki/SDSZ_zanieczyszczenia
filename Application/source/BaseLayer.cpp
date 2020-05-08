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
        
        data[x*size + y] = 4000;
    }
	float tex[] = {1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	texture = context->device.CreateImage(VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_SAMPLED_BIT, 2, 2, Library::GRAPHICS, tex, 4*sizeof(float));
	atlas = Library::TextureAtlas(&texture, 2, 2);

    map = new Library::Map(context, size, size, data.data(), VK_FORMAT_R32_SFLOAT, sizeof(float));

    for(int i = 0; i < NKWADRATOW; i++)
    {
		rect[i].UseTexture(&atlas);
		rect[i].UseTile(rand()%2, rand()%2);
        rect[i].SetPosition({(double)rand()/RAND_MAX * 2 - 1, (double)rand()/RAND_MAX * 2 - 1, 0});
        rect[i].SetScale({0.1, 0.1, 1.0});
        rect[i].SetRotation((double)rand()/RAND_MAX * 3.141592 * 2.0);
    }
}

void BaseLayer::onDetach()
{
	map->CleanUp();
    context->device.DestroyImage(texture);
}

void BaseLayer::Update()
{
    map->DispatchCompute(false);
}

void BaseLayer::Render()
{
    map->Render();

    for(int i = 0; i < NKWADRATOW; i++)
    {
        renderer->Render(rect[i]);
    }


}