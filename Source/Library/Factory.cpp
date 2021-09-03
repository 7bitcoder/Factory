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
        auto res = _workers.emplace(data.id, std::make_unique<Worker>(data));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Worker of id {} was already created.", data.id));
        }
    }

    void Factory::addLoadingRamp(const LoadingRampData &data)
    {
        auto res = _loadingRamps.emplace(data.id, std::make_unique<LoadingRamp>(data));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Loading ramp of id {} was already created.", data.id));
        }
    }

    void Factory::addStorehouse(const StoreHouseData &data)
    {
        auto res = _storeHouses.emplace(data.id, std::make_unique<StoreHouse>(data));
        if (!res.second)
        {
            throw std::runtime_error(std::format("Storehouse of id {} was already created.", data.id));
        }
    }

    void Factory::addLink(const LinkData &data)
    {
        SourceNode::RawPtr sourceNode;
        DestinationNode::RawPtr destinationNode;

        if (data.source.type == NodeType::STORE)
        {
            throw std::runtime_error("Storehouse cannot be used as link source.");
        }
        if (data.source.type == NodeType::RAMP)
        {
            if (auto found = _loadingRamps.find(data.source.id); found != _loadingRamps.end())
            {
                sourceNode = found->second.get();
            }
            else
            {
                throw std::runtime_error(std::format("Could not find LoadingRamp of id {} to be link source.", data.source.id));
            }
        }
        else // Worker
        {
            if (auto found = _workers.find(data.source.id); found != _workers.end())
            {
                sourceNode = found->second.get();
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Worker of id {} to be link source.", data.source.id));
            }
        }

        if (data.destination.type == NodeType::RAMP)
        {
            throw std::runtime_error("LoadingRamp cannot be used as link destination.");
        }
        if (data.destination.type == NodeType::STORE)
        {
            if (auto found = _storeHouses.find(data.destination.id); found != _storeHouses.end())
            {
                destinationNode = found->second.get();
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Storehouse of id {} to be link destination.", data.destination.id));
            }
        }
        else // Worker
        {
            if (auto found = _workers.find(data.destination.id); found != _workers.end())
            {
                destinationNode = found->second.get();
            }
            else
            {
                throw std::runtime_error(std::format("Could not find Worker of id {} to be link destination.", data.destination.id));
            }
        }

        auto link = std::make_shared<Link>(data, *sourceNode, *destinationNode);
        auto res = _links.emplace(data.id, link);
        if (!res.second)
        {
            throw std::runtime_error(std::format("Link of id {} was already created.", data.id));
        }
        sourceNode->bindSourceLink(link);
        destinationNode->bindDestinationLink(link);
    }

    void Factory::removeWorker(size_t id)
    {
        if (auto workerPair = _workers.find(id); workerPair != _workers.end())
        {
            auto &worker = workerPair->second;
            worker->unbindAllDestinations();
            worker->unbindAllSources();
            _workers.erase(workerPair);
            removeExpiredLinks();
        }
    }

    void Factory::removeLoadingRamp(size_t id)
    {
        if (auto rampPair = _loadingRamps.find(id); rampPair != _loadingRamps.end())
        {
            auto &ramp = rampPair->second;
            ramp->unbindAllSources();
            _loadingRamps.erase(rampPair);
            removeExpiredLinks();
        }
    }

    void Factory::removeStorehouse(size_t id)
    {
        if (auto storePair = _storeHouses.find(id); storePair != _storeHouses.end())
        {
            auto &store = storePair->second;
            store->unbindAllDestinations();
            _storeHouses.erase(storePair);
            removeExpiredLinks();
        }
    }

    void Factory::removeLink(size_t id)
    {
        if (auto linkPair = _links.find(id); linkPair != _links.end())
        {
            auto link = linkPair->second.lock();
            if (link)
            {
                link->unBindDestination();
                link->unBindSource();
            }
            _links.erase(linkPair);
        }
    }

    const std::vector<WorkerData> Factory::getWorkersData() const
    {
        std::vector<WorkerData> res;
        res.reserve(_workers.size());
        for (auto &worker : _workers)
        {
            res.push_back(worker.second->getWorkerData());
        }
        return res;
    }

    const std::vector<LoadingRampData> Factory::getLoadingRampsData() const
    {
        std::vector<LoadingRampData> res;
        res.reserve(_loadingRamps.size());
        for (auto &ramps : _loadingRamps)
        {
            res.push_back(ramps.second->getLoadingRampData());
        }
        return res;
    }

    const std::vector<StoreHouseData> Factory::getStorehousesData() const
    {
        std::vector<StoreHouseData> res;
        res.reserve(_storeHouses.size());
        for (auto &store : _storeHouses)
        {
            res.push_back(store.second->getStoreHouseData());
        }
        return res;
    }

    const std::vector<LinkData> Factory::getLinksData() const
    {
        std::vector<LinkData> res;
        res.reserve(_links.size());
        for (auto &linkWeak : _links)
        {
            auto link = linkWeak.second.lock();
            if (link)
            {
                res.push_back(link->getLinkData());
            }
        }
        return res;
    }

    size_t Factory::removeExpiredLinks()
    {
        return std::erase_if(_links, [](const std::pair<const size_t, sd::Link::WeakPtr> &item)
                             { return item.second.expired(); });
    }

    void Factory::validate() const
    {
        for (auto &workerPair : _workers)
        {
            auto &worker = workerPair.second;
            if (!worker->connectedSources())
            {
                throw std::runtime_error(std::format("Worker of id {} is not connected as source.", worker->getId()));
            }
            if (!worker->connectedSources())
            {
                throw std::runtime_error(std::format("Worker of id {} is not connected as destination.", worker->getId()));
            }
        }

        for (auto &rampPair : _loadingRamps)
        {
            auto &ramp = rampPair.second;
            if (!ramp->connectedSources())
            {
                throw std::runtime_error(std::format("Ramp of id {} is not connected as source.", ramp->getId()));
            }
        }

        for (auto &storeHousePair : _storeHouses)
        {
            auto &store = storeHousePair.second;
            if (!store->connectedDestinations())
            {
                throw std::runtime_error(std::format("StoreHouse of id {} is not connected as destination.", store->getId()));
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
        for (auto &storeHousePair : _storeHouses)
        {
            auto &store = storeHousePair.second;
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
        for (auto &storeHousePair : _storeHouses)
        {
            auto &store = storeHousePair.second;
            out << store->getStructureRaport(0) << dEnd{};
        }
        return out.str();
    }

    bool Factory::initialized() const { return !_loadingRamps.empty() || !_workers.empty() || !_storeHouses.empty() || !_links.empty(); }

    void Factory::run(size_t maxIterations, std::ostream &raportOutStream, const RaportGuard &raportGuard)
    {
        raportOutStream << "========= Factory Structure ========" << std::endl;
        raportOutStream << generateStructureRaport() << std::endl;
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