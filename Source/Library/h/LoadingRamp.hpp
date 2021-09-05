#pragma once

#include "Link.hpp"
#include "Node.hpp"
#include "Processable.hpp"
#include "Product.hpp"
#include "Utils.hpp"


namespace sd
{

    struct LoadingRampData
    {
        size_t id;
        size_t deliveryInterval;
    };

    class LoadingRamp final : public SourceNode, public Processable
    {
      public:
        using Ptr = std::unique_ptr<LoadingRamp>;

        LoadingRamp(size_t id, size_t deliveryInterval = 1);

        LoadingRamp(const LoadingRampData &data);

        const LoadingRampData getLoadingRampData() const;

        std::string getStructureRaport(size_t offset) const final;

        std::string toString() const final;

        NodeType getNodeType() const final;

      protected:
        void triggerOperation() final;

      private:
        Product::Ptr createProduct() const;
    };
} // namespace sd