#pragma once

#include "Drawable.h"

namespace Library
{
    std::vector<glm::vec3> vertices = {
    glm::vec3(-0.5, -0.5, 0.0),
    glm::vec3(0.5, -0.5, 0.0),
    glm::vec3(0.5, 0.5, 0.0),
    glm::vec3(-0.5, 0.5, 0.0)};

    std::vector<unsigned int> indices = 
        {0, 1, 2, 0, 2, 3};

    class Rectangle : public Drawable
    {
        public:
            void Draw();
            void SetPosition();
            void SetScale();
            void SetRotation();

        private:
            static Model ConstructUnitSquare(); //Tworzy kwadrat o środku w (0,0) i boku 1
            Model model;
            Image texture;
    };
}



