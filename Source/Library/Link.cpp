#pragma once

#include "Link.hpp"
#include "Node.hpp"

namespace sd
{
    Link::Link(size_t id, double probability, std::shared_ptr<SinkNode> node)
        : Identifiable(id), _probability(probability), _sink(node)
    {
    }

    void Link::passProduct(Product::Ptr &&product)
    {
        _sink->moveIn(std::move(product));
    }
}