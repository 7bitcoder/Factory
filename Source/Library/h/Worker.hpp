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

        Worker(size_t id, WorkerType type = WorkerType::FIFO, size_t processingTime = 1);

        Product::Ptr moveOutProduct() final;

        void moveInProduct(Product::Ptr &&product) final;
    };
}