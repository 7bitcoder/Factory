#include <format>

#include "Net.hpp"

namespace sd
{
    void Net::build()
    {
        _loadingRamps = {{1, std::make_shared<LoadingRamp>(1)}};
        _workers = {{1, std::make_shared<Worker>(1)}};
        _storeHauses = {{1, std::make_shared<StoreHause>(1)}};
        _links = {{1, std::make_shared<Link>(1, 1, _loadingRamps[1], _workers[1])}, {2, std::make_shared<Link>(2, 1, _workers[1], _storeHauses[1])}};
        _loadingRamps[1]->getSourceLinksHub().bindLink(_links[1]);
        _workers[1]->getSourceLinksHub().bindLink(_links[2]);
        size_t time = 0;
        while (time++ < 100)
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

    void Net::addWorker(size_t id, size_t processingTime, QueueType queueType)
    {
        auto res = _workers.emplace(id, {id, processingTime, queueType});
        if (!res.second)
        {
            throw std::runtime_error(std::format("Worker of id {} was already created.", id));
        }
    }

    void Net::addLoadingRamp(size_t id, size_t deliveryInterval)
    {
        auto res = _loadingRamps.emplace(std::make_pair{{id}, {id, deliveryInterval}});
        if (!res.second)
        {
            throw std::runtime_error(std::format("Loading ramp of id {} was already created.", id));
        }
    }

    void Net::addStorehause(size_t id)
    {
        auto res = _loadingRamps.emplace(id, id);
        if (!res.second)
        {
            throw std::runtime_error(std::format("Storehause of id {} was already created.", id));
        }
    }

    void Net::addLink(LinkBind source, LinkBind sink, double probability)
    {
    }
}