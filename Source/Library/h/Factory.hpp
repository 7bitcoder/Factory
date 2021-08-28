#pragma once
#include <iostream>
#include <filesystem>
#include <unordered_map>

#include "Link.hpp"
#include "Worker.hpp"
#include "LoadingRamp.hpp"
#include "StoreHause.hpp"

namespace sd
{
    class Factory
    {
    private:
        std::unordered_map<size_t, LoadingRamp::Ptr> _loadingRamps;
        std::unordered_map<size_t, Worker::Ptr> _workers;
        std::unordered_map<size_t, StoreHause::Ptr> _storeHauses;
        std::unordered_map<size_t, Link::Ptr> _links;

    public:
        void load(const std::filesystem::path &file) {}

        void build() {
            _loadingRamps = { {1, std::make_shared<LoadingRamp>(1)}};
            _workers = { {1, std::make_shared<Worker>(1)}};
            _storeHauses = { {1, std::make_shared<StoreHause>(1)}};
            _links = { {1, std::make_shared<Link>(1, 1, _workers[1])}, {2, std::make_shared<Link>(2, 1, _storeHauses[1])}};
            _loadingRamps[1]->getSourceLinksHub().registerLink(_links[1]);
            _workers[1]->getSourceLinksHub().registerLink(_links[2]);
            size_t time = 0;
            while(time++ < 100) {
                for(auto& ramp: _loadingRamps) {
                    ramp.second->proceed(time);
                }
                for(auto& ramp: _workers) {
                    ramp.second->proceed(time);
                }
            }
        }
    };
}