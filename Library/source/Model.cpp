#include "Model.h"

namespace Library
{

    Model::Model()
    {}

    Model::Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices)
        : vertices(vertices), indices(indices)
    {}

}