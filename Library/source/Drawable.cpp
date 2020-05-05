#include "Drawable.h"

namespace Library
{

    Drawable::Drawable()
        : position(0, 0, 0), scale(1, 1, 1), rotation(0)
    {}

    glm::mat4 Drawable::ApplyTransforms()
    {
		glm::mat4 transformation(1.0);
        glm::mat4 rotation = glm::rotate(glm::mat4(1), this->rotation, glm::vec3(0.0, 0.0, 1.0));
        glm::mat4 scale  = glm::scale(glm::mat4(1), this->scale);
        glm::mat4 translation = glm::translate(glm::mat4(1), position);
        transformation = translation * rotation * scale;
        return transformation;
    }
}
