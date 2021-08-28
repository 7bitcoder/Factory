#pragma once

#include "Node.hpp"
#include "Link.hpp"

namespace sd
{
    enum WorkerType
    {
        LIFO,
        FIFO
    };

    class Worker final : public SourceNode, public SinkNode
    {
    private:
        WorkerType _type;
        Product::Ptr _currentProduct;

    public:
        using Ptr = std::shared_ptr<Worker>;

        Worker(size_t id, WorkerType type = WorkerType::FIFO, size_t processingTime = 1)
            : SourceNode(id, processingTime), SinkNode(id), _type(type) {}

        Product::Ptr moveOut() final
        {
            auto ptr = std::move(_currentProduct);
            if (!empty())
            {
                _currentProduct = std::move(getProduct(_type == WorkerType::FIFO));
            }
            return std::move(ptr);
        }

        void moveIn(Product::Ptr &&product) final
        {
            SinkNode::moveIn(std::move(product));
            if (!_currentProduct)
            {
                _currentProduct = std::move(getProduct(_type == WorkerType::FIFO));
                reset();
            }
        }
    };
}