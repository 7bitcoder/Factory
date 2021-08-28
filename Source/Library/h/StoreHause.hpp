#pragma once

#include <deque>

#include "Node.hpp"
#include "Product.hpp"

namespace sd
{
    class StoreHause : public SinkNode
    {
    private:

    public:
        using Ptr = std::shared_ptr<StoreHause>;

        StoreHause(size_t id) : SinkNode(id) {}
    };
}