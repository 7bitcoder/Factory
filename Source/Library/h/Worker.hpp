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

    struct WorkerData
    {
        size_t id;
        size_t processingTime;
        WorkerType type;
    };

    class Worker final : public SourceNode, public SinkNode
    {
    private:
        WorkerType _type;
        Product::Ptr _currentProduct;

    public:
        using Ptr = std::shared_ptr<Worker>;

        Worker(size_t id, WorkerType type = WorkerType::FIFO, size_t processingTime = 1);

        Worker(const WorkerData &data);

        Product::Ptr moveOutProduct() final;

        void moveInProduct(Product::Ptr &&product) final;

        std::string getStructureRaport(size_t offset) final;

        std::string getStateRaport(size_t offset) final;

        std::string getCurrentWorkRaport();

        std::string toString() final;
        std::string getStructure() final;

        NodeType getNodeType() final { return NodeType::WORKER; }

    private:
        std::string queueTypeAsStr() const;
    };
}