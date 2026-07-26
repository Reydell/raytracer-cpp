#pragma once

#include "Vector.hpp"
#include <memory>

#include <vector>

struct Box {
    Vector min;
    Vector max;
    Vector centroid;

    Box operator|(const Box& other) const;
    Box& operator|=(const Box& other);
};

struct Node {
    Box box;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    
    std::vector<uint32_t> objectIds;

public:
    bool IsLeaf() const;
};