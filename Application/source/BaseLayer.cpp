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
    int size = 1024;     //Rozdzielczosc obrazka do obliczen

    std::cout<<"Attached!"<<std::endl;
    data.resize(size*size);

    srand(time(nullptr));
    memset(data.data(), 0, size*size*sizeof(float));
    for(int i = 0; i < 10; i++)
    {
        int x = rand()%size;
        int y = rand()%size;
        
        data[x*size + y] = 4.0;
    }
    Library::ImageFile imageFile("Resources/plik.png");
	unsigned char* tex = imageFile.GetData();
	texture = context->device.CreateImage(VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT,
        imageFile.GetWidth(), imageFile.GetHeight(), Library::GRAPHICS, tex, 4*sizeof(unsigned char));
	atlas = Library::TextureAtlas(&texture, 1, 1);
    imageFile.Close();

    map = new Library::Map(context, size, size, data.data(), VK_FORMAT_R32_SFLOAT, sizeof(float));
    float difficulty[] = {0.0f};
    float wind[] = {0.3f, -0.3f, 0.3, -0.3,
                    -0.3, -0.3, -0.3, 0.3};
    map->SetDifficultyMap(1, 1, difficulty, VK_FORMAT_R32_SFLOAT, sizeof(float));
    map->SetWind(2, 2, wind, VK_FORMAT_R32G32_SFLOAT, 2*sizeof(float));
    map->SetSimulationProperties(1.0f, 50.0f, 1.0f);

    font = new Library::Font("Resources/Roboto-Regular.ttf", context->GetFreetype());
    fontImage = context->device.CreateImage(VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT, 8*FONT_SIZE, 8*FONT_SIZE, Library::GRAPHICS, font->GetData(), 4*sizeof(unsigned char));
    fontTextureAtlas = Library::TextureAtlas(&fontImage, 8, 8);
    text = new Library::Text(font, &fontTextureAtlas, renderer, "Czytelny fragment\ntekstu w programie", {-0.9, 0.9}, 0.1);

    for(int i = 0; i < NKWADRATOW; i++)
    {
		rect[i].UseTexture(&atlas);
		rect[i].UseTile(0,0);
        rect[i].SetPosition({(double)rand()/RAND_MAX * 2 - 1, (double)rand()/RAND_MAX * 2 - 1, 0});
        rect[i].SetScale({0.1, 0.1, 1.0});
    }

}

void BaseLayer::onDetach()
{
    delete font;
	map->CleanUp();
    context->device.DestroyImage(texture);
    context->device.DestroyImage(fontImage);
}

static float timeElapsed = 0.0f;
static float samples;

void BaseLayer::Update(float frameTime)
{
	switch(state)
    {
        case SIMULATING:

        map->DispatchCompute(frameTime);
        sampleNumber++;
        samples += 1.0f;
        timeElapsed += Library::Events::GetFrameTime();
        if(timeElapsed > 1.0f)
        {
            samplesPS = samples;
            samples = 0.0f;
            timeElapsed = 0.0f;
        }
        if(Library::Events::KeyPressed(GLFW_KEY_SPACE))
        {
            state = PAUSED;
        }
        break;
        case PAUSED:
            if(Library::Events::KeyPressed(GLFW_KEY_R))
            {
                state = SIMULATING;
            }
        break;
    }
    text->UpdateText(std::string("Sample number - " + Library::Text::FloatToString(float(sampleNumber)/1000.0,2)) + "k\nWith " + Library::Text::FloatToString(samplesPS, 0) + " samples per second");
    
}

void BaseLayer::Render()
{
    map->Render();
    text->Render();
    glm::vec2 mousePos = Library::Events::GetNormalizedMousePosition();
    rect[0].SetPosition({mousePos.x, mousePos.y, 0.0});
    for(uint32_t i = 0; i < NKWADRATOW; i++)
    {
        //renderer->Render(rect[i]);
    }
}