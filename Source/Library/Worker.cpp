#pragma once

#include <sstream>
#include <format>

#include "Worker.hpp"

namespace sd
{
    Worker::Worker(size_t id, WorkerType type, size_t processingTime)
        : SourceNode(id, processingTime), SinkNode(id), _type(type), Node(id) {}

    Worker::Worker(const WorkerData &data)
        : SourceNode(data.id, data.processingTime), SinkNode(data.id), _type(data.type), Node(data.id) {}

    Product::Ptr Worker::moveOutProduct()
    {
        auto ptr = std::move(_currentProduct);
        if (!areProductsAvailable())
        {
            _currentProduct = std::move(getProduct(_type == WorkerType::FIFO));
        }
        return std::move(ptr);
    }

    void Worker::moveInProduct(Product::Ptr &&product)
    {
        SinkNode::moveInProduct(std::move(product));
        if (!_currentProduct)
        {
            _currentProduct = std::move(getProduct(_type == WorkerType::FIFO));
            resetProcessTime();
        }
    }

    std::string Worker::getStructureRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset++) << toString() << std::endl;
        out << getOffset(offset) << "Processing time: " << getProcesingTime() << std::endl;
        out << getOffset(offset) << "Queue type: " << sd::toString(getWorkerType()) << std::endl;
        out << getSourceLinksHub().getStructureRaport(offset);
        return out.str();
    }

    std::string Worker::getStateRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset) << toString() << std::endl;
        out << getOffset(++offset) << "Queue: " << getCurrentWorkRaport() << getSinkRaport() << std::endl;
        return out.str();
    };

    std::string Worker::getCurrentWorkRaport() const
    {
        return _currentProduct ? std::format("{} (pt = {}), ", _currentProduct->toString(), getCurrentProcesingTime()) : "";
    };

    std::string Worker::toString() const { return std::format("WORKER #{}", getId()); }

    NodeType Worker::getNodeType() const { return NodeType::WORKER; }

    WorkerType Worker::getWorkerType() const { return _type; }

    const WorkerData Worker::getWorkerData() const { return {getId(), getProcesingTime(), getWorkerType()}; }
}