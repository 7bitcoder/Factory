#pragma once

#include "Worker.hpp"

namespace sd
{
    Worker::Worker(size_t id, WorkerType type, size_t processingTime)
        : SourceNode(id, processingTime), SinkNode(id), _type(type) {}

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
}