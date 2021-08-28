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
    public:
        enum QueueType
        {
            LIFO,
            FIFO
        };

        enum NodeType
        {
            RAMP,
            WORKER,
            STORE
        };

        struct LinkBind
        {
            size_t id;
            NodeType type;
        };

    private:
        std::unordered_map<size_t, LoadingRamp::Ptr> _loadingRamps;
        std::unordered_map<size_t, Worker::Ptr> _workers;
        std::unordered_map<size_t, StoreHause::Ptr> _storeHauses;
        std::unordered_map<size_t, Link::Ptr> _links;

    public:
        void build();

        void addWorker(size_t id, size_t processingTime, QueueType queueType);
        void addLoadingRamp(size_t id, size_t deliveryInterval);
        void addStorehause(size_t id);

        void addLink(LinkBind source, LinkBind sink, double probability);
    };
}