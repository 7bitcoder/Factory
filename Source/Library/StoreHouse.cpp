#include <format>
#include <sstream>


#include "StoreHouse.hpp"

namespace sd
{
    StoreHouse::StoreHouse(size_t id) : DestinationNode(id), Node(id)
    {
    }

    StoreHouse::StoreHouse(const StoreHouseData &data) : StoreHouse(data.id)
    {
    }

    std::string StoreHouse::getStructureRaport(size_t offset) const
    {
        return toString();
    }

    std::string StoreHouse::getStateRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset) << toString() << std::endl;
        out << getOffset(++offset) << "Queue: " << DestinationNode::getStateRaport(offset);
        return out.str();
    };

    std::string StoreHouse::toString() const
    {
        return std::format("STOREHOUSE #{}", getId());
    }

    NodeType StoreHouse::getNodeType() const
    {
        return NodeType::STORE;
    }

    const StoreHouseData StoreHouse::getStoreHouseData() const
    {
        return {getId()};
    }
} // namespace sd