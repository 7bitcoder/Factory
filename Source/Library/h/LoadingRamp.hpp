#pragma once

#include "Product.hpp"
#include "Link.hpp"
#include "Node.hpp"

namespace sd
{

    struct LoadingRampData
    {
        size_t id;
        size_t deliveryInterval;
    };

    class LoadingRamp final : public SourceNode
    {
    public:
        using Ptr = std::shared_ptr<LoadingRamp>;

        LoadingRamp(size_t id, size_t deliveryInterval = 1);

        LoadingRamp(const LoadingRampData &data);

        Product::Ptr moveOutProduct() final;

        std::string getStructureRaport(size_t offset) final;

        Product::Ptr createProduct();

        std::string toString() final;
        std::string getStructure() final;

        NodeType getNodeType() final { return NodeType::RAMP; }
    };
}