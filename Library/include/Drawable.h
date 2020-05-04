#pragma once

#include "Model.h"
#include "Image.h"

namespace Library
{
    class Drawable
    {
        public:
            void Draw();

            void SetPosition(glm::vec3 position);
            void SetScale(glm::vec3 scale);
            void SetRotation(float angle);
        private:
            glm::mat4 ApplyTransforms();
            Model model;
            Image texture;
            glm::vec3 position;
            glm::vec3 scale;
            float rotation;
    };
    
}