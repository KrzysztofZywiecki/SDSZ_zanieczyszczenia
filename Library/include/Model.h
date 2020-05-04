#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Library
{

    class Model
    {
        public:
            struct Vertex
            {
                glm::vec3 position;
                glm::vec2 tex_coord;
            };
            Model();
            Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices);
            
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
    };

}