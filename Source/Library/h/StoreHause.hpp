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

    class StoreHause : public SinkNode
    {
    private:
    public:
        using Ptr = std::shared_ptr<StoreHause>;

        StoreHause(size_t id);

        StoreHause(const StoreHauseData& data);

        std::string getStructureRaport(size_t offset) final;

        std::string getStateRaport(size_t offset) final;

        std::string toString() final;
        std::string getStructure() final;

        NodeType getNodeType() final { return NodeType::STORE; }
    };
}