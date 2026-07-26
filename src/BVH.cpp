#include "BVH.hpp"
#include <algorithm>
#include <cstddef>

bool Node::IsLeaf() const {
    return (left == nullptr) && (right == nullptr);
}

Box& Box::operator|=(const Box& other) {
    min[0] = std::min(min[0], other.min[0]);
    min[1] = std::min(min[1], other.min[1]);
    min[2] = std::min(min[2], other.min[2]);

    max[0] = std::max(max[0], other.max[0]);
    max[1] = std::max(max[1], other.max[1]);
    max[2] = std::max(max[2], other.max[2]);

    return *this;
}
