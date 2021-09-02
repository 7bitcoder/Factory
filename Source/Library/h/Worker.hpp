#pragma once

#include "Node.hpp"
#include "Link.hpp"

namespace sd
{

    struct WorkerData
    {
        size_t id;
        size_t processingTime;
        WorkerType type;
    };

    class Worker final : public SourceNode, public DestinationNode
    {
    private:
        WorkerType _type;
        Product::Ptr _currentProduct;

    public:
        using Ptr = std::unique_ptr<Worker>;

        Worker(size_t id, WorkerType type = WorkerType::FIFO, size_t processingTime = 1);

        Worker(const WorkerData &data);

        const WorkerData getWorkerData() const;

        Product::Ptr moveOutProduct() final;

        void moveInProduct(Product::Ptr &&product) final;

        std::string getStructureRaport(size_t offset) const final;

        std::string getStateRaport(size_t offset) const final;

        std::string toString() const final;

        NodeType getNodeType() const final;

        bool isProcessingProduct() const;
    private:
        WorkerType getWorkerType() const;

        std::string getCurrentWorkRaport() const;

        void getNextProductToProcess();
    };
}