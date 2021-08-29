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
    class SinkNode;
    class Worker;
    class StoreHause;

    class Random
    {
    private:
        std::random_device _rd;
        std::default_random_engine _eng;
        std::uniform_real_distribution<double> _distr;
        Random();

    public:
        static Random &get();
        double nextDouble();
    };

    class Link final
        : public Identifiable,
          public StructureRaportable,
          public Structure,
          public std::enable_shared_from_this<Link>
    {
    private:
        static size_t _idSeed;
        double _probability;
        const double _baseProbability;
        std::weak_ptr<SourceNode> _source;
        std::shared_ptr<SinkNode> _sink;

    public:
        using Ptr = std::shared_ptr<Link>;
        using WeakPtr = std::weak_ptr<Link>;

        Link(double probability, std::shared_ptr<SourceNode> source, std::shared_ptr<SinkNode> sink);
        ~Link();

        void bindLinks();

        void passProduct(Product::Ptr &&product);

        double getBaseProbability() const;

        double getProbability() const;

        void setProbability(double newProbability);

        std::string getStructureRaport(size_t offset) final;

        std::string getStructure() final;

        std::shared_ptr<SinkNode> getSink() const;
    };

    class SourceLinksHub : public StructureRaportable
    {
    private:
        std::vector<Link::Ptr> _links;

    public:
        void bindLink(Link::Ptr link);
        void unBindLink(size_t id);
        void unBindLink(Link::Ptr link);

        void passProduct(Product::Ptr &&product);

        bool connected() const;

        std::string getStructureRaport(size_t offset) final;

        const std::vector<Link::Ptr> &getLinks() const;

    private:
        void normalize();

    private:
        Link::Ptr getRandomLink() const;
    };

    class SinkLinksHub
    {
    private:
        std::vector<Link::WeakPtr> _links;

    public:
        void bindLink(Link::Ptr link);
        void unBindLink(size_t id);
        void unBindLink(Link::Ptr link);

        bool connected() const;
    };
}