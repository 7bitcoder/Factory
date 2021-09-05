#include <sstream>
#include <format>

#include "LoadingRamp.hpp"

namespace sd
{
    LoadingRamp::LoadingRamp(size_t id, size_t deliveryInterval)
        : SourceNode(id), Processable(deliveryInterval), Node(id) {}

    LoadingRamp::LoadingRamp(const LoadingRampData &data)
        : LoadingRamp(data.id, data.deliveryInterval) {}

    std::string LoadingRamp::getStructureRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset++) << toString() << std::endl;
        out << getOffset(offset) << "Delivery interval: " << getTotalProcesingTime() << std::endl;
        out << SourceNode::getStructureRaport(offset);
        return out.str();
    }

    std::string LoadingRamp::toString() const { return std::format("LOADING_RAMP #{}", getId()); }

    NodeType LoadingRamp::getNodeType() const { return NodeType::RAMP; }

    const LoadingRampData LoadingRamp::getLoadingRampData() const { return {getId(), getTotalProcesingTime()}; }

    void LoadingRamp::triggerOperation() { setProduct(std::move(createProduct())); }

    Product::Ptr LoadingRamp::createProduct() const { return std::make_unique<Product>(); }
}