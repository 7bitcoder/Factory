#pragma once

#include <sstream>

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

    Node::Node(size_t id) : Identifiable(id) {}

    SourceNode::SourceNode(size_t id, size_t processTime) : Node(id), _processTime(processTime) {}

    size_t SourceNode::getProcesingTime() const { return _processTime; }

    size_t SourceNode::getCurrentProcesingTime() const { return _currentProcessTime; }

    void SourceNode::process(const size_t currentTime)
    {
        if (++_currentProcessTime >= _processTime)
        {
            auto ptr = moveOutProduct();
            if (ptr)
            {
                passProduct(std::move(ptr));
            }
            resetProcessTime();
        }
    }

    void SourceNode::unbindAllSources()
    {
        for (auto &link : _links)
        {
            link->unBindDestination();
        }
    }

    void SourceNode::bindSourceLink(Link::Ptr link)
    {
        _links.emplace_back(link);
        normalize();
    }

    void SourceNode::unBindSourceLink(size_t id)
    {
        _links.erase(std::remove_if(_links.begin(), _links.end(), [id](Link::Ptr ptr)
                                    { return ptr->getId() == id; }),
                     _links.end());
        normalize();
    }

    void SourceNode::passProduct(Product::Ptr &&product)
    {
        auto link = getRandomLink();
        if (!link)
        {
            throw std::runtime_error("No links available");
        }
        link->getDestination().moveInProduct(std::move(product));
    }

    bool SourceNode::connectedSources() const { return !_links.empty(); }

    Link::Ptr SourceNode::getRandomLink() const
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

    void SourceNode::normalize()
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

    std::string SourceNode::getStructureRaport(size_t offset) const
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

    void SourceNode::resetProcessTime() { _currentProcessTime = 0; }

    DestinationNode::DestinationNode(size_t id) : Node(id) {}

    void DestinationNode::moveInProduct(Product::Ptr &&product)
    {
        _storedProducts.emplace_back(std::move(product));
    }

    bool DestinationNode::areProductsAvailable() const { return !_storedProducts.empty(); }

    Product::Ptr DestinationNode::getProduct(bool first)
    {
        if (!areProductsAvailable())
        {
            throw std::runtime_error("Destination is empty");
        }
        Product::Ptr result;
        if (first)
        {
            result = std::move(_storedProducts.front());
            _storedProducts.pop_front();
        }
        else
        {
            result = std::move(_storedProducts.back());
            _storedProducts.pop_back();
        }
        return std::move(result);
    }

    std::string DestinationNode::getStoredProductsRaport() const
    {
        std::stringstream out;
        for (auto &product : _storedProducts)
        {
            out << product->toString();
            if (product != _storedProducts.back())
            {
                out << ", ";
            }
        }
        return out.str();
    }

    void DestinationNode::unbindAllDestinations()
    {
        for (auto &link : _links)
        {
            link->unBindSource();
        }
    }

    void DestinationNode::bindDestinationLink(Link::Ptr link) { _links.emplace_back(link); }

    void DestinationNode::unBindDestinationLink(size_t id)
    {
        _links.erase(std::remove_if(_links.begin(), _links.end(), [id](Link::Ptr ptr)
                                    { return ptr->getId() == id; }),
                     _links.end());
    }

    bool DestinationNode::connectedDestinations() const { return !_links.empty(); }
}