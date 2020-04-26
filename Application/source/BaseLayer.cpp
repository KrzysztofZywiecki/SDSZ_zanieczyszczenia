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
    data.resize(256*256);

    memset(data.data(), 0, 256*256*sizeof(float));
    data[128*256+128] = 4000;
    data[30*256+200] = 4000;
    data[150*256+50] = 4000;
    data[200*256+20] = 4000;
    data[90*256+190] = 4000;
    data[40*256+30] = 4000;

    map = new Library::Map(context, 256, 256, data.data(), VK_FORMAT_R32_SFLOAT, sizeof(float));
}

void BaseLayer::onDetach()
{
	map->CleanUp();
}

void BaseLayer::Update()
{
    map->DispatchCompute(false);
}

void BaseLayer::Render()
{
    map->Render();
}