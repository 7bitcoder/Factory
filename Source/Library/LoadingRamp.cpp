#pragma once

#include <sstream>
#include <format>

#include "LoadingRamp.hpp"

namespace sd
{
    LoadingRamp::LoadingRamp(size_t id, size_t deliveryInterval)
        : SourceNode(id, deliveryInterval), Node(id) {}

    Product::Ptr LoadingRamp::moveOutProduct()
    {
        return std::move(createProduct());
    }

    Product::Ptr LoadingRamp::createProduct()
    {
        return std::make_unique<Product>();
    }

    std::string LoadingRamp::getStructureRaport(size_t offset)
    {
        std::stringstream out;
        out << getOffset(offset++) << toString() << std::endl;
        out << getOffset(offset) << "Delivery interval: " << getProcesingTime() << std::endl;
        out << getSourceLinksHub().getStructureRaport(offset);
        return out.str();
    }

    std::string LoadingRamp::getStructure() { return std::format("LOADING_RAMP id={} delivery-interval={}", getId(), getProcesingTime()); }

    std::string LoadingRamp::toString() { return std::format("LOADING_RAMP #{}", getId()); }
}