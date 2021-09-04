#pragma once

#include <random>
#include <iomanip>
#include <vector>

#include "Interfaces.hpp"
#include "Identifiable.hpp"
#include "Product.hpp"

namespace sd
{
    class SourceNode;
    class DestinationNode;
    class Worker;
    class StoreHouse;

    struct LinkBind
    {
        size_t id;
        NodeType type;
    };

    struct LinkData
    {
        size_t id;
        double probability;
        LinkBind source;
        LinkBind destination;
    };

    class Link final
        : public Identifiable,
          public IStructureRaportable
    {
    private:
        const double _baseProbability;
        double _probability;
        SourceNode &_source;
        DestinationNode &_destination;

    public:
        using Ptr = std::shared_ptr<Link>;
        using WeakPtr = std::weak_ptr<Link>;

        Link(size_t id, double probability, SourceNode &source, DestinationNode &destination);
        Link(const LinkData &data, SourceNode &source, DestinationNode &destination);

        const LinkData getLinkData() const;

        SourceNode &getSource();
        DestinationNode &getDestination();

        std::string getStructureRaport(size_t offset) const final;

        double getBaseProbability() const;

        double getProbability() const;
        void setProbability(double newProbability);

        void unBindSource();
        void unBindDestination();
    };
}