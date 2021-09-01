#pragma once

#include <sstream>
#include <format>

#include "LoadingRamp.hpp"

namespace sd
{
    LoadingRamp::LoadingRamp(size_t id, size_t deliveryInterval)
        : SourceNode(id, deliveryInterval), Node(id) {}

    LoadingRamp::LoadingRamp(const LoadingRampData &data)
        : SourceNode(data.id, data.deliveryInterval), Node(data.id) {}

    Product::Ptr LoadingRamp::moveOutProduct()
    {
        return std::move(createProduct());
    }

    Product::Ptr LoadingRamp::createProduct() const
    {
        return std::make_unique<Product>();
    }

    std::string LoadingRamp::getStructureRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset++) << toString() << std::endl;
        out << getOffset(offset) << "Delivery interval: " << getProcesingTime() << std::endl;
        out << getSourceLinksHub().getStructureRaport(offset);
        return out.str();
    }

    std::string LoadingRamp::toString() const { return std::format("LOADING_RAMP #{}", getId()); }

    NodeType LoadingRamp::getNodeType() const { return NodeType::RAMP; }

    const LoadingRampData LoadingRamp::getLoadingRampData() const { return {getId(), getProcesingTime()}; }
}