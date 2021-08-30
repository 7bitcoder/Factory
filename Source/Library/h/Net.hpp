#pragma once
#include <iostream>
#include <filesystem>
#include <map>
#include <variant>

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

        struct LinkBind
        {
            size_t id;
            NodeType type;
        };

        class RaportInfo
        {
        private:
            using Interval = size_t;
            struct RaportTimes
            {
                std::vector<size_t> raportTimes;
                mutable size_t nextRaportIndex;
            };
            std::variant<Interval, RaportTimes> _raportTimes;

        public:
            RaportInfo(std::variant<size_t, std::vector<size_t>> &var);

            bool isRaportTime(size_t currentIteration) const;
        };

    private:
        std::map<size_t, LoadingRamp::Ptr> _loadingRamps;
        std::map<size_t, Worker::Ptr> _workers;
        std::map<size_t, StoreHause::Ptr> _storeHauses;
        std::map<size_t, Link::Ptr> _links;

    public:
        void build();
        void run(size_t maxIterations, std::ostream &out, const RaportInfo &raportInfo);

        std::string generateStateRaport();
        std::string generateStructureRaport();
        std::string getStructure();

        void addWorker(size_t id, size_t processingTime, QueueType queueType);
        void addLoadingRamp(size_t id, size_t deliveryInterval);
        void addStorehause(size_t id);

        void addLink(size_t id, double probability, LinkBind source, LinkBind sink);

        void validate();
    };
}