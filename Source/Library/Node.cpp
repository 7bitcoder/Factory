#pragma once

#include <sstream>

#include "Node.hpp"
#include "Random.hpp"

namespace sd
{
    Node::Node(size_t id) : Identifiable(id) {}

    SourceNode::SourceNode(size_t id) : Node(id) {}

    void SourceNode::setProduct(Product::Ptr &&product)
    {
        if (_product)
        {
            throw std::runtime_error("Simulation Error");
        }
        _product = std::move(product);
    }

    void SourceNode::passProduct()
    {
        if (!isProductReady())
        {
            return;
        }
        auto link = getRandomLink();
        if (!link)
        {
            throw std::runtime_error("No links available");
        }
        link->getDestination().addProductToStore(std::move(_product));
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

    bool SourceNode::connectedSources() const { return !_links.empty(); }

    void SourceNode::unbindAllSources()
    {
        for (auto &link : _links)
        {
            link->unBindDestination();
        }
    }

    bool SourceNode::isProductReady() const { return bool{_product}; }

    Link::Ptr SourceNode::getRandomLink() const
    {
        if (_links.empty())
        {
            return nullptr;
        }
        double propability = Random::get().next();
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

    DestinationNode::DestinationNode(size_t id) : Node(id) {}

    void DestinationNode::addProductToStore(Product::Ptr &&product)
    {
        _storedProducts.emplace_back(std::move(product));
    }

    Product::Ptr DestinationNode::getStoredProduct(bool first)
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

    std::string DestinationNode::getStateRaport(size_t offset) const
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

    void DestinationNode::bindDestinationLink(Link::Ptr link) { _links.emplace_back(link); }

    void DestinationNode::unBindDestinationLink(size_t id)
    {
        _links.erase(std::remove_if(_links.begin(), _links.end(), [id](Link::Ptr ptr)
                                    { return ptr->getId() == id; }),
                     _links.end());
    }

    bool DestinationNode::connectedDestinations() const { return !_links.empty(); }

    void DestinationNode::unbindAllDestinations()
    {
        for (auto &link : _links)
        {
            link->unBindSource();
        }
    }

    bool DestinationNode::areProductsAvailable() const { return getStoredProductsSize() > 0; }

    size_t DestinationNode::getStoredProductsSize() const { return _storedProducts.size(); }
}