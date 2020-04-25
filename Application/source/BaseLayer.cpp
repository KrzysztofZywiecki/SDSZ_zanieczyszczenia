#include "BaseLayer.h"

#include <iostream>

std::vector<float> data;

BaseLayer::BaseLayer(Library::Context* context)
	:context(context)
{

}

void BaseLayer::onAttach()
{
    std::cout<<"Attached!"<<std::endl;
    data.resize(100*100);

    memset(data.data(), 0, 100*100*sizeof(float));
    data[50*100+50] = 100;
    
    map = new Library::Map(context, 100, 100, data.data(), VK_FORMAT_R32_SFLOAT, sizeof(float));
}

void BaseLayer::onDetach()
{
	map->CleanUp();
}

void BaseLayer::Update()
{
}

void BaseLayer::Render()
{
}