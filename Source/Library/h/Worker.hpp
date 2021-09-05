#pragma once

#include "Link.hpp"
#include "Node.hpp"
#include "Processable.hpp"


namespace sd
{

    struct WorkerData
    {
        size_t id;
        size_t processingTime;
        WorkerType type;
    };

    class Worker final : public SourceNode, public DestinationNode, public Processable
    {
      private:
        WorkerType _type;
        Product::Ptr _currentProduct;

      public:
        using Ptr = std::unique_ptr<Worker>;

        Worker(size_t id, WorkerType type = WorkerType::FIFO, size_t processingTime = 1);

        Worker(const WorkerData &data);

        const WorkerData getWorkerData() const;

        void process(const size_t currentTime) final;

        std::string getStructureRaport(size_t offset) const final;

        std::string getStateRaport(size_t offset) const final;

        std::string toString() const final;

        NodeType getNodeType() const final;

        bool isProcessingProduct() const;

      protected:
        void triggerOperation() final;

      private:
        WorkerType getWorkerType() const;

        std::string getCurrentWorkRaport() const;
    };
} // namespace sd