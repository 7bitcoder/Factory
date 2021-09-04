#pragma once
#include <iostream>
#include <filesystem>
#include <map>
#include <variant>

#include "Link.hpp"
#include "Worker.hpp"
#include "LoadingRamp.hpp"
#include "StoreHouse.hpp"

namespace sd
{
    class Factory
    {
    public:
        class RaportGuard
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
            RaportGuard(const std::variant<size_t, std::vector<size_t>> &var);

            bool isRaportTime(size_t currentIteration) const;
        };

    private:
        std::map<size_t, LoadingRamp::Ptr> _loadingRamps;
        std::map<size_t, Worker::Ptr> _workers;
        std::map<size_t, StoreHouse::Ptr> _storeHouses;
        std::map<size_t, Link::WeakPtr> _links;

    public:
        using Ptr = std::unique_ptr<Factory>;

        void run(size_t maxIterations, std::ostream &raportOutStream, const RaportGuard &raportGuard);

        std::string generateStateRaport();
        std::string generateStructureRaport();

        void addWorker(const WorkerData &data);
        void addLoadingRamp(const LoadingRampData &data);
        void addStorehouse(const StoreHouseData &data);
        void addLink(const LinkData &data);

        void removeWorker(size_t id);
        void removeLoadingRamp(size_t id);
        void removeStorehouse(size_t id);
        void removeLink(size_t id);

        const std::vector<WorkerData> getWorkersData() const;
        const std::vector<LoadingRampData> getLoadingRampsData() const;
        const std::vector<StoreHouseData> getStorehousesData() const;
        const std::vector<LinkData> getLinksData() const;

        bool initialized() const;
        void validate() const;

    private:
        size_t removeExpiredLinks();
    };
}