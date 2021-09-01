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

        const LoadingRampData getLoadingRampData() const;

        Product::Ptr moveOutProduct() final;

        std::string getStructureRaport(size_t offset) const final;

        std::string toString() const final;

        NodeType getNodeType() const final;

    private:
        Product::Ptr createProduct() const;
    };
}