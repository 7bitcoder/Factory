#pragma once

#include <sstream>

#include "Node.hpp"

namespace sd
{
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
                _sourceLinksHub.passProduct(std::move(ptr));
            }
            resetProcessTime();
        }
    }

    void SourceNode::resetProcessTime() { _currentProcessTime = 0; }

    SourceLinksHub &SourceNode::getSourceLinksHub() { return _sourceLinksHub; }

    SinkNode::SinkNode(size_t id) : Node(id) {}

    void SinkNode::moveInProduct(Product::Ptr &&product)
    {
        _storedProducts.emplace_back(std::move(product));
    }

    SinkLinksHub &SinkNode::getSinkLinksHub() { return _sinkLinksHub; }

    bool SinkNode::areProductsAvailable() const { return _storedProducts.empty(); }

    Product::Ptr SinkNode::getProduct(bool first)
    {
        if (areProductsAvailable())
        {
            throw std::runtime_error("Sink is empty");
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

    std::string SinkNode::getSinkRaport() const
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
}