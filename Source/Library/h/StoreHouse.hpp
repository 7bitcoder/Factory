#pragma once

#include <deque>

#include "Node.hpp"
#include "Product.hpp"

namespace sd
{
    struct StoreHouseData
    {
        size_t id;
    };

    class StoreHouse final : public DestinationNode
    {
      public:
        using Ptr = std::unique_ptr<StoreHouse>;

        StoreHouse(size_t id);

        StoreHouse(const StoreHouseData &data);

        const StoreHouseData getStoreHouseData() const;

        std::string getStructureRaport(size_t offset) const final;

        std::string getStateRaport(size_t offset) const final;

        std::string toString() const final;

        NodeType getNodeType() const final;
    };
} // namespace sd