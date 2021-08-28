#pragma once

#include "Product.hpp"
#include "Link.hpp"
#include "Node.hpp"

namespace sd
{
    class LoadingRamp final : public SourceNode
    {
    public:
        using Ptr = std::shared_ptr<LoadingRamp>;

        LoadingRamp(size_t id, size_t deliveryInterval = 1)
            : SourceNode(id, deliveryInterval) {}

        Product::Ptr moveOut() final
        {
            return std::move(createProduct());
        }

        Product::Ptr createProduct()
        {
            return std::make_unique<Product>();
        }
    };
}