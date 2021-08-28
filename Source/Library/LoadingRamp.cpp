#pragma once

#include "LoadingRamp.hpp"

namespace sd
{
    LoadingRamp::LoadingRamp(size_t id, size_t deliveryInterval)
        : SourceNode(id, deliveryInterval) {}

    Product::Ptr LoadingRamp::moveOutProduct()
    {
        return std::move(createProduct());
    }

    Product::Ptr LoadingRamp::createProduct()
    {
        return std::make_unique<Product>();
    }
}