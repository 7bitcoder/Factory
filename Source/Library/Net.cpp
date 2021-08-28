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

}