#pragma once

#include <deque>

#include "Node.hpp"
#include "Product.hpp"

namespace sd
{
    struct StoreHauseData
    {
        size_t id;
    };

    class StoreHause final : public SinkNode
    {
    public:
        using Ptr = std::shared_ptr<StoreHause>;

        StoreHause(size_t id);

        StoreHause(const StoreHauseData &data);

        const StoreHauseData getStoreHauseData() const;

        std::string getStructureRaport(size_t offset) const final;

        std::string getStateRaport(size_t offset) const final;

        std::string toString() const final;

        NodeType getNodeType() const final;
    };
}