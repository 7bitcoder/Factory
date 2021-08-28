#pragma once
#include <random>
#include <iomanip>
#include <vector>

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

    class Link : public Identifiable
    {
    private:
        double _probability;
        const double _baseProbability;
        std::weak_ptr<SourceNode> _source;
        std::shared_ptr<SinkNode> _sink;

    public:
        using Ptr = std::shared_ptr<Link>;
        using WeakPtr = std::weak_ptr<Link>;

        Link(size_t id, double probability, std::shared_ptr<SourceNode> source, std::shared_ptr<SinkNode> sink);

        void passProduct(Product::Ptr &&product);

        double getBaseProbability() const;

        double getProbability() const;

        void setProbability(double newProbability);
    };

    class SourceLinksHub
    {
    private:
        std::vector<Link::Ptr> _links;

    public:
        void bindLink(Link::Ptr link);
        void unBindLink(size_t id);
        void unBindLink(Link::Ptr link);

        void passProduct(Product::Ptr &&product);

        bool connected() const;

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