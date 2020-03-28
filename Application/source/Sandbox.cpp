#include "Sandbox.h"

Sandbox::Sandbox()
{
    layerStack.PushLayer(new BaseLayer());
}

Sandbox::~Sandbox()
{

}

Library::Application* Library::CreateApplication()
{
    return new Sandbox();
}
