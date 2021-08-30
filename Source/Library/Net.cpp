#include <format>

#include "Net.hpp"

namespace sd
{
    namespace
    {
        struct dEnd
        {
        };

        std::ostream &operator<<(std::ostream &out, const dEnd &t)
        {
            out << std::endl
                << std::endl;
            return out;
        }
    }

    Net::RaportInfo::RaportInfo(std::variant<size_t, std::vector<size_t>> &var)
    {
        if (size_t *intervalPtr = std::get_if<size_t>(&var))
        {
            _raportTimes = *intervalPtr;
        }
        else if (std::vector<size_t> *raportTimesPtr = std::get_if<std::vector<size_t>>(&var))
        {
            auto &raportTimes = *raportTimesPtr;
            _raportTimes = RaportTimes{std::move(raportTimes), 0};

            auto &raportTimesVar = std::get<RaportTimes>(_raportTimes);
            auto &raportTimesVec = raportTimesVar.raportTimes;

            if (raportTimesVec.empty())
            {
                throw std::runtime_error("Raport Times vector cannot be empty");
            }

            std::sort(raportTimesVec.begin(), raportTimesVec.end());

            auto last = std::unique(raportTimesVec.begin(), raportTimesVec.end());
            raportTimesVec.erase(last, raportTimesVec.end());
        }
    }

    bool Net::RaportInfo::isRaportTime(size_t currentIteration) const
    {
        if (const Interval *interval = std::get_if<Interval>(&_raportTimes))
        {
            if(*interval == 0) {
                return false;
            }
            return currentIteration % *interval;
        }
        else if (const RaportTimes *raportTimesPtr = std::get_if<RaportTimes>(&_raportTimes))
        {
            auto &raportTimes = *raportTimesPtr;
            auto &raportTimesVector = raportTimes.raportTimes;
            auto &nextRaportIndex = raportTimes.nextRaportIndex;
            if (nextRaportIndex < raportTimesVector.size() && currentIteration == raportTimesVector.at(nextRaportIndex))
            {
                ++nextRaportIndex;
                return true;
            }
        }
        else
        {
            throw std::runtime_error("Raport Info Error");
        }
        return false;
    }

    void Net::build()
    {
        addLoadingRamp(1, 1);
        addWorker(1, 1, QueueType::FIFO);
        addStorehause(1);
        addLink(1, 1, {1, NodeType::RAMP}, {1, NodeType::WORKER});
        addLink(2, 1, {1, NodeType::WORKER}, {1, NodeType::STORE});
    }

    void Net::addWorker(size_t id, size_t processingTime, QueueType queueType)
    {
        auto workerType = queueType == QueueType::FIFO ? WorkerType::FIFO : WorkerType::FIFO;
        auto res = _workers.emplace(id, std::make_shared<Worker>(id, workerType, processingTime));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Worker of id {} was already created.", id));
        }
    }

    void Net::addLoadingRamp(size_t id, size_t deliveryInterval)
    {
        auto res = _loadingRamps.emplace(id, std::make_shared<LoadingRamp>(id, deliveryInterval));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Loading ramp of id {} was already created.", id));
        }
    }

    void Net::addStorehause(size_t id)
    {
        auto res = _storeHauses.emplace(id, std::make_shared<StoreHause>(id));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Storehause of id {} was already created.", id));
        }
    }

    void Net::addLink(size_t id, double probability, LinkBind source, LinkBind sink)
    {
        SourceNode::Ptr sourceNode;
        SinkNode::Ptr sinkNode;

        if (source.type == NodeType::STORE)
        {
            throw std::runtime_error(std::format("Storehause of id {} cannot be used as link source.", source.id));
        }
        if (source.type == NodeType::RAMP)
        {
            if (auto found = _loadingRamps.find(source.id); found != _loadingRamps.end())
            {
                sourceNode = found->second;
            }
            else
            {
                throw std::runtime_error(std::format("Could not find LoadingRamp of id {} to be link source ", source.id));
            }
        }
        else // Worker
        {
            if (auto found = _workers.find(source.id); found != _workers.end())
            {
                sourceNode = found->second;
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Worker of id {} to be link source ", source.id));
            }
        }

        if (sink.type == NodeType::RAMP)
        {
            throw std::runtime_error(std::format("LoadingRamp of id {} cannot be used as link sink.", sink.id));
        }
        if (sink.type == NodeType::STORE)
        {
            if (auto found = _storeHauses.find(sink.id); found != _storeHauses.end())
            {
                sinkNode = found->second;
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Storehause of id {} to be link sink.", sink.id));
            }
        }
        else // Worker
        {
            if (auto found = _workers.find(sink.id); found != _workers.end())
            {
                sinkNode = found->second;
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Worker of id {} to be link sink.", source.id));
            }
        }

        auto res = _links.emplace(id, std::make_shared<Link>(id, probability, sourceNode, sinkNode));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Worker of id {} was already created.", id));
        }
        res.first->second->bindLinks();
    }

    void Net::validate()
    {
        for (auto &workerPair : _workers)
        {
            auto &worker = workerPair.second;
            if (!worker->getSourceLinksHub().connected())
            {
                throw std::runtime_error(std::format("Worker of id {} is not connected as source.", worker->getId()));
            }
            if (!worker->getSinkLinksHub().connected())
            {
                throw std::runtime_error(std::format("Worker of id {} is not connected as sink.", worker->getId()));
            }
        }

        for (auto &rampPair : _loadingRamps)
        {
            auto &ramp = rampPair.second;
            if (!ramp->getSourceLinksHub().connected())
            {
                throw std::runtime_error(std::format("Ramp of id {} is not connected as source.", ramp->getId()));
            }
        }

        for (auto &storeHausePair : _storeHauses)
        {
            auto &store = storeHausePair.second;
            if (!store->getSinkLinksHub().connected())
            {
                throw std::runtime_error(std::format("StoreHause of id {} is not connected as sink.", store->getId()));
            }
        }
    }

    std::string Net::generateStateRaport()
    {
        std::stringstream out;
        out << "== WORKERS ==" << dEnd{};
        for (auto &workerPair : _workers)
        {
            auto &worker = workerPair.second;
            out << worker->getStateRaport(0) << dEnd{};
        }
        out << "== STOREHOUSES ==" << dEnd{};
        for (auto &storeHausePair : _storeHauses)
        {
            auto &store = storeHausePair.second;
            out << store->getStateRaport(0) << dEnd{};
        }
        return out.str();
    }

    std::string Net::generateStructureRaport()
    {
        std::stringstream out;
        out << "== LOADING RAMPS ==" << dEnd{};
        for (auto &rampPair : _loadingRamps)
        {
            auto &ramp = rampPair.second;
            out << ramp->getStructureRaport(0) << dEnd{};
        }
        out << "== WORKERS ==" << dEnd{};
        for (auto &workerPair : _workers)
        {
            auto &worker = workerPair.second;
            out << worker->getStructureRaport(0) << dEnd{};
        }
        out << "== STOREHOUSES ==" << dEnd{};
        for (auto &storeHausePair : _storeHauses)
        {
            auto &store = storeHausePair.second;
            out << store->getStructureRaport(0) << dEnd{};
        }
        return out.str();
    }

    std::string Net::getStructure()
    {
        std::stringstream out;
        out << "; == LOADING RAMPS ==" << dEnd{};
        for (auto &rampPair : _loadingRamps)
        {
            auto &ramp = rampPair.second;
            out << ramp->getStructure() << dEnd{};
        }
        out << "; == WORKERS ==" << dEnd{};
        for (auto &workerPair : _workers)
        {
            auto &worker = workerPair.second;
            out << worker->getStructure() << dEnd{};
        }
        out << "; == STOREHOUSES ==" << dEnd{};
        for (auto &storeHausePair : _storeHauses)
        {
            auto &store = storeHausePair.second;
            out << store->getStructure() << dEnd{};
        }

        out << "; == LINKS ==" << dEnd{};
        for (auto &linkPair : _links)
        {
            auto &link = linkPair.second;
            out << link->getStructure() << dEnd{};
        }
        return out.str();
    }

    void Net::run(size_t maxIterations, std::ostream &out, const RaportInfo &raportInfo)
    {
        size_t time = 0;
        while (time++ < maxIterations)
        {
            for (auto &ramp : _loadingRamps)
            {
                ramp.second->process(time);
            }
            for (auto &ramp : _workers)
            {
                ramp.second->process(time);
            }

        }
    }
}