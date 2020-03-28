#include "BaseLayer.h"

#include <iostream>

void BaseLayer::onAttach()
{
    std::cout<<"Attached!"<<std::endl;
}

void BaseLayer::onDetach()
{
    std::cout<<"Detached!"<<std::endl;
}

void BaseLayer::Update()
{
    std::cout<<"Updated!"<<std::endl;
}

void BaseLayer::Render()
{
    std::cout<<"Rendered"<<std::endl;
}