#include <format>
#include <sstream>

#include "Worker.hpp"

namespace sd
{
    Worker::Worker(size_t id, WorkerType type, size_t processingTime)
        : SourceNode(id), Processable(processingTime), DestinationNode(id), _type(type), Node(id)
    {
    }

    Worker::Worker(const WorkerData &data) : Worker(data.id, data.type, data.processingTime)
    {
    }

    bool Worker::isProcessingProduct() const
    {
        return bool{_currentProduct};
    }

    std::string Worker::getStructureRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset++) << toString() << std::endl;
        out << getOffset(offset) << "Processing time: " << getTotalProcesingTime() << std::endl;
        out << getOffset(offset) << "Queue type: " << sd::toString(getWorkerType()) << std::endl;
        out << SourceNode::getStructureRaport(offset);
        return out.str();
    }

    std::string Worker::getStateRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset) << toString() << std::endl;
        out << getOffset(++offset) << "Queue: " << getCurrentWorkRaport() << DestinationNode::getStateRaport(offset);
        return out.str();
    };

    std::string Worker::getCurrentWorkRaport() const
    {
        return _currentProduct ? std::format("{} (pt = {}), ", _currentProduct->toString(), getCurrentProcesingTime())
                               : "";
    };

    std::string Worker::toString() const
    {
        return std::format("WORKER #{}", getId());
    }

    NodeType Worker::getNodeType() const
    {
        return NodeType::WORKER;
    }

    void Worker::process(const size_t currentTime)
    {
        if (!isProcessingProduct())
        {
            if (areProductsAvailable())
            {
                _currentProduct = std::move(getStoredProduct(_type == WorkerType::FIFO));
                reset();
            }
        }
        if (isProcessingProduct())
        {
            Processable::process(currentTime);
        }
    }

    void Worker::triggerOperation()
    {
        setProduct(std::move(_currentProduct));
        if (areProductsAvailable())
        {
            _currentProduct = std::move(getStoredProduct(_type == WorkerType::FIFO));
            reset();
        }
        else
        {
            stop();
        }
    }

    WorkerType Worker::getWorkerType() const
    {
        return _type;
    }

    const WorkerData Worker::getWorkerData() const
    {
        return {getId(), getTotalProcesingTime(), getWorkerType()};
    }
} // namespace sd