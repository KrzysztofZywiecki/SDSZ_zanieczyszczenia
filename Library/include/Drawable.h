#pragma once

#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Image.h"
#include "Context.h"

namespace Library
{
    class Drawable
    {
        public:
            Drawable();
            void SetPosition(glm::vec3 position){this->position = position;}
            void SetScale(glm::vec3 scale){this->scale = scale;}
            void SetRotation(float angle){this->rotation = angle;}

            glm::mat4 ApplyTransforms();
        private:
            Model model;
            glm::vec3 position;
            glm::vec3 scale;
            float rotation;
    };
    
}