#pragma once
#include <random>
#include <iomanip>

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

        Random() : _eng(_rd()), _distr(0, 1) {}

    public:
        static Random &get()
        {
            static Random r;
            return r;
        }

        double nextDouble() { return _distr(_eng); }
    };

    class Link : public Identifiable
    {
    private:
        double _probability;
        std::weak_ptr<SourceNode> _source;
        std::shared_ptr<SinkNode> _sink;

    public:
        using Ptr = std::shared_ptr<Link>;
        using WeakPtr = std::weak_ptr<Link>;

        Link(size_t id, double probability, std::shared_ptr<SinkNode> node);

        void passProduct(Product::Ptr &&product);

        double getProbability() const { return _probability; }
    };

    class SourceLinksHub
    {
    private:
        std::vector<Link::Ptr> _links;

    public:
        void registerLink(Link::Ptr link)
        {
            _links.emplace_back(link);
        }

        void passProduct(Product::Ptr &&product)
        {
            auto link = getRandomLink();
            if (!link)
            {
                throw std::runtime_error("No links available");
            }
            link->passProduct(std::move(product));
        }

        bool connected() const { return !_links.empty(); }

    private:
        Link::Ptr getRandomLink()
        {
            if (_links.empty())
            {
                return nullptr;
            }
            double propability = Random::get().nextDouble();
            double accumulate = 0;
            for (auto &link : _links)
            {
                accumulate += link->getProbability();
                if (propability <= accumulate)
                {
                    return link;
                }
            }
            return _links.back();
        }
    };

    class SinkLinksHub
    {
    private:
        std::vector<Link::WeakPtr> _links;

    public:
        void registerLink(Link::Ptr link)
        {
            _links.emplace_back(link);
        }

        bool connected() const { return !_links.empty(); }
    };
}