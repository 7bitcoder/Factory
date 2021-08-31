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
            RaportGuard(std::variant<size_t, std::vector<size_t>> &var);

            bool isRaportTime(size_t currentIteration) const;
        };

    private:
        std::map<size_t, LoadingRamp::Ptr> _loadingRamps;
        std::map<size_t, Worker::Ptr> _workers;
        std::map<size_t, StoreHause::Ptr> _storeHauses;
        std::map<size_t, Link::Ptr> _links;

    public:
        using Ptr = std::unique_ptr<Factory>;
        static Factory::Ptr fromStream(std::istream &in);

        void run(size_t maxIterations, std::ostream &raportOutStream, const RaportGuard &raportInfo);

        std::string generateStateRaport();
        std::string generateStructureRaport();
        std::string getStructure();

        void addWorker(const WorkerData &data);
        void addLoadingRamp(const LoadingRampData &data);
        void addStorehause(const StoreHauseData &data);
        void addLink(const LinkData &data);

        bool empty();
        void validate();
    };
}