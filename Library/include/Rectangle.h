#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Library
{

    class Rectangle
    {
        public:
            std::vector<glm::vec3> vertices = {
                glm::vec3(-0.5, -0.5, 0.0),
                glm::vec3(0.5, -0.5, 0.0),
                glm::vec3(0.5, 0.5, 0.0),
                glm::vec3(-0.5, 0.5, 0.0)};

            std::vector<unsigned int> indices = 
                {0, 1, 2, 0, 2, 3};


        private:
        
    };
}



