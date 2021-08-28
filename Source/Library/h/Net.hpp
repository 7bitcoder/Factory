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
    class Net
    {
    private:
        std::unordered_map<size_t, LoadingRamp::Ptr> _loadingRamps;
        std::unordered_map<size_t, Worker::Ptr> _workers;
        std::unordered_map<size_t, StoreHause::Ptr> _storeHauses;
        std::unordered_map<size_t, Link::Ptr> _links;

    public:
        void build();
    };
}