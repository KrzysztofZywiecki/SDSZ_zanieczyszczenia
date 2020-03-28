#include "Application.h"

extern Library::Application* Library::CreateApplication();

int main()
{
	Library::Application* app = Library::CreateApplication();
	app->Run();
	delete app;

	return 0;
}