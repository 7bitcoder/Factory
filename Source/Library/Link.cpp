#pragma once

#include <algorithm>
#include <format>
#include <sstream>

#include "Link.hpp"
#include "Node.hpp"

namespace sd
{

    namespace
    {
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
    }

    Link::Link(size_t id, double probability, std::shared_ptr<SourceNode> source, std::shared_ptr<SinkNode> sink)
        : Identifiable(id), _baseProbability(probability), _probability(probability), _source(source), _sink(sink) {}

    Link::Link(const LinkData &data, std::shared_ptr<SourceNode> source, std::shared_ptr<SinkNode> sink)
        : Identifiable(data.id), _baseProbability(data.probability), _probability(data.probability), _source(source), _sink(sink) {}

    const LinkData Link::getLinkData() const
    {
        auto source = getSource();
        auto sink = getSink();
        return {getId(), getProbability(), {source->getId(), source->getNodeType()}, {sink->getId(), sink->getNodeType()}};
    }

    Link::~Link()
    {
        unBindSource();
        unBindSink();
    }

    void Link::bindLinks()
    {
        getSource()->getSourceLinksHub().bindLink(shared_from_this());
        getSink()->getSinkLinksHub().bindLink(shared_from_this());
    }

    void Link::unBindSource()
    {
        if (!_source.expired())
        {
            getSource()->getSourceLinksHub().unBindLink(getId());
        }
    }

    void Link::unBindSink()
    {
        if (!_sink.expired())
        {
            getSink()->getSinkLinksHub().unBindLink(getId());
        }
    }

    bool Link::connected() const { return !_source.expired() && !_sink.expired(); }

    void Link::passProduct(Product::Ptr &&product) { getSink()->moveInProduct(std::move(product)); }

    double Link::getProbability() const { return _probability; }

    double Link::getBaseProbability() const { return _baseProbability; }

    void Link::setProbability(double newProbability) { _probability = newProbability; }

    std::string Link::getStructureRaport(size_t offset) const { return std::format("{}{} (p = {})", getOffset(offset), getSink()->toString(), getProbability()); }

    std::shared_ptr<SourceNode> Link::getSource() const
    {
        if (_source.expired())
        {
            throw std::runtime_error(std::format("Link id: {}, source node is invalid", getId()));
        }
        return _source.lock();
    }

    std::shared_ptr<SinkNode> Link::getSink() const
    {
        if (_sink.expired())
        {
            throw std::runtime_error(std::format("Link id: {}, sink node is invalid", getId()));
        }
        return _sink.lock();
    }

    SourceLinksHub::~SourceLinksHub()
    {
        unbindAll();
    }

    void SourceLinksHub::unbindAll()
    {
        for (auto &link : _links)
        {
            link->unBindSink();
        }
    }

    void SourceLinksHub::bindLink(Link::Ptr link)
    {
        _links.emplace_back(link);
        normalize();
    }

    void SourceLinksHub::unBindLink(size_t id)
    {
        std::erase_if(_links, [id](Link::Ptr link)
                      { return link->getId() == id; });
        normalize();
    }

    void SourceLinksHub::unBindLink(Link::Ptr linkToRemove)
    {
        std::erase_if(_links, [linkToRemove](Link::Ptr link)
                      { return link == linkToRemove; });
        normalize();
    }

    void SourceLinksHub::passProduct(Product::Ptr &&product)
    {
        auto link = getRandomLink();
        if (!link)
        {
            throw std::runtime_error("No links available");
        }
        link->passProduct(std::move(product));
    }

    bool SourceLinksHub::connected() const { return !_links.empty(); }

    Link::Ptr SourceLinksHub::getRandomLink() const
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

    void SourceLinksHub::normalize()
    {

        double totalPropability = 0;
        for (auto &link : _links)
        {
            totalPropability += link->getBaseProbability();
        }
        for (auto &link : _links)
        {
            auto baseProbability = link->getBaseProbability();
            link->setProbability(baseProbability / totalPropability);
        }
    }

    std::string SourceLinksHub::getStructureRaport(size_t offset) const
    {
        std::stringstream out;
        out << getOffset(offset++) << "Receivers:" << std::endl;
        for (auto &link : _links)
        {
            out << link->getStructureRaport(offset);
            if (link != _links.back())
            {
                out << std::endl;
            }
        }
        return out.str();
    }

    SinkLinksHub::~SinkLinksHub() { unbindAll(); }

    void SinkLinksHub::unbindAll()
    {
        for (auto &link : _links)
        {
            link->unBindSource();
        }
    }

    void SinkLinksHub::bindLink(Link::Ptr link) { _links.emplace_back(link); }

    void SinkLinksHub::unBindLink(size_t id)
    {
        std::erase_if(_links, [id](Link::Ptr ptr)
                      { return ptr->getId() == id; });
    }

    void SinkLinksHub::unBindLink(Link::Ptr linkToRemove)
    {
        std::erase_if(_links, [linkToRemove](Link::Ptr ptr)
                      { return ptr == linkToRemove; });
    }

    bool SinkLinksHub::connected() const { return !_links.empty(); }
}