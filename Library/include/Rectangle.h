#pragma once

#include <glm/glm.hpp>

namespace Library
{

    class Rectangle
    {
        public:
            Rectangle(glm::vec2 bl, glm::vec2 tr);
            Rectangle(){};

            bool IsInside(glm::vec2 point);

            glm::vec2 bottomLeft;
            glm::vec2 topRight;
        private:

    };

}