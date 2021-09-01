#include <format>

#include "Factory.hpp"

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

    Factory::RaportGuard::RaportGuard(std::variant<size_t, std::vector<size_t>> &var)
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

    bool Factory::RaportGuard::isRaportTime(size_t currentIteration) const
    {
        if (const Interval *interval = std::get_if<Interval>(&_raportTimes))
        {
            if (*interval == 0)
            {
                return false;
            }
            return (currentIteration % *interval) == 0;
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

    void Factory::addWorker(const WorkerData &data)
    {
        auto res = _workers.emplace(data.id, std::make_shared<Worker>(data));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Worker of id {} was already created.", data.id));
        }
    }

    void Factory::addLoadingRamp(const LoadingRampData &data)
    {
        auto res = _loadingRamps.emplace(data.id, std::make_shared<LoadingRamp>(data));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Loading ramp of id {} was already created.", data.id));
        }
    }

    void Factory::addStorehause(const StoreHauseData &data)
    {
        auto res = _storeHauses.emplace(data.id, std::make_shared<StoreHause>(data));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Storehause of id {} was already created.", data.id));
        }
    }

    void Factory::addLink(const LinkData &data)
    {
        SourceNode::Ptr sourceNode;
        SinkNode::Ptr sinkNode;

        if (data.source.type == NodeType::STORE)
        {
            throw std::runtime_error(std::format("Storehause of id {} cannot be used as link source.", data.source.id));
        }
        if (data.source.type == NodeType::RAMP)
        {
            if (auto found = _loadingRamps.find(data.source.id); found != _loadingRamps.end())
            {
                sourceNode = found->second;
            }
            else
            {
                throw std::runtime_error(std::format("Could not find LoadingRamp of id {} to be link source ", data.source.id));
            }
        }
        else // Worker
        {
            if (auto found = _workers.find(data.source.id); found != _workers.end())
            {
                sourceNode = found->second;
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Worker of id {} to be link source ", data.source.id));
            }
        }

        if (data.sink.type == NodeType::RAMP)
        {
            throw std::runtime_error(std::format("LoadingRamp of id {} cannot be used as link sink.", data.sink.id));
        }
        if (data.sink.type == NodeType::STORE)
        {
            if (auto found = _storeHauses.find(data.sink.id); found != _storeHauses.end())
            {
                sinkNode = found->second;
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Storehause of id {} to be link sink.", data.sink.id));
            }
        }
        else // Worker
        {
            if (auto found = _workers.find(data.sink.id); found != _workers.end())
            {
                sinkNode = found->second;
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Worker of id {} to be link sink.", data.source.id));
            }
        }

        auto res = _links.emplace(data.id, std::make_shared<Link>(data, sourceNode, sinkNode));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Worker of id {} was already created.", data.id));
        }
        res.first->second->bindLinks();
    }

    void Factory::removeWorker(size_t id)
    {
        _workers.erase(id);
    }

    void Factory::removeLoadingRamp(size_t id)
    {
        _loadingRamps.erase(id);
    }

    void Factory::removeStorehause(size_t id)
    {
        _storeHauses.erase(id);
    }

    void Factory::removeLink(size_t id)
    {
        _links.erase(id);
    }

    const std::vector<WorkerData> Factory::getWorkersData() const
    {
        auto res = std::vector<WorkerData>(_workers.size());
        for (auto &worker : _workers)
        {
            res.emplace_back(worker.second->getWorkerData());
        }
        return res;
    }

    const std::vector<LoadingRampData> Factory::getLoadingRampsData() const
    {
        auto res = std::vector<LoadingRampData>(_loadingRamps.size());
        for (auto &ramps : _loadingRamps)
        {
            res.emplace_back(ramps.second->getLoadingRampData());
        }
        return res;
    }

    const std::vector<StoreHauseData> Factory::getStorehausesData() const
    {
        auto res = std::vector<StoreHauseData>(_storeHauses.size());
        for (auto &store : _storeHauses)
        {
            res.emplace_back(store.second->getStoreHauseData());
        }
        return res;
    }

    const std::vector<LinkData> Factory::getLinksData() const
    {
        auto res = std::vector<LinkData>(_links.size());
        for (auto &link : _links)
        {
            res.emplace_back(link.second->getLinkData());
        }
        return res;
    }

    void Factory::validate() const
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

    std::string Factory::generateStateRaport()
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

    std::string Factory::generateStructureRaport()
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

    bool Factory::initialized() const { return _loadingRamps.empty() && _workers.empty() && _storeHauses.empty() && _links.empty(); }

    void Factory::run(size_t maxIterations, std::ostream &raportOutStream, const RaportGuard &raportGuard)
    {
        raportOutStream << "========= Simulation Start =========" << std::endl;
        for (size_t time = 0; time < maxIterations; ++time)
        {
            for (auto &ramp : _loadingRamps)
            {
                ramp.second->process(time);
            }
            for (auto &ramp : _workers)
            {
                ramp.second->process(time);
            }
            if (raportGuard.isRaportTime(time))
            {
                raportOutStream << std::format("========= Iteration: {} =========", time) << std::endl;
                raportOutStream << generateStateRaport();
            }
        }
    }
}