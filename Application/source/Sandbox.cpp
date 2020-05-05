#include "Sandbox.h"

Sandbox::Sandbox()
{
    layerStack.PushLayer(new BaseLayer(&context, &renderer));
}

Sandbox::~Sandbox()
{

}

Library::Application* Library::CreateApplication()
{
    return new Sandbox();
}
