#include "Rectangle.h"

namespace Library
{

    Rectangle::Rectangle( glm::vec2 bl, glm::vec2 tr )
        : bottomLeft(bl), topRight(tr)
    {}

    bool Rectangle::IsInside(glm::vec2 point)
    {
        return ((point.x > bottomLeft.x) && (point.x < topRight.x))
            && ((point.y > bottomLeft.y) && (point.y < topRight.y));
    }

}