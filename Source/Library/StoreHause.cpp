#pragma once

#include <sstream>
#include <format>

#include "StoreHause.hpp"

namespace sd
{
    StoreHause::StoreHause(size_t id) : SinkNode(id), Node(id) {}

    StoreHause::StoreHause(const StoreHauseData &data) : SinkNode(data.id), Node(data.id) {}

    std::string StoreHause::getStructureRaport(size_t offset) const
    {
        std::stringstream out;
        out << toString() << std::endl;
        return out.str();
    }

    std::string StoreHause::getStateRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset) << toString() << std::endl;
        out << getOffset(++offset) << "Queue: " << getSinkRaport() << std::endl;
        return out.str();
    };

    std::string StoreHause::toString() const { return std::format("STOREHOUSE #{}", getId()); }

    NodeType StoreHause::getNodeType() const { return NodeType::STORE; }

    const StoreHauseData StoreHause::getStoreHauseData() const { return {getId()}; }
}