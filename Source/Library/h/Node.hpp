#pragma once
#include <memory>
#include <deque>

#include "Identifiable.hpp"
#include "Product.hpp"
#include "Link.hpp"

namespace sd
{
    struct ProductSource
    {
        virtual Product::Ptr moveOut() = 0;
    };

    struct ProductSink
    {
        virtual void moveIn(Product::Ptr &&product) = 0;
    };

    class Node : public Identifiable
    {
    public:
        using Ptr = std::shared_ptr<Node>;
        Node(size_t id) : Identifiable(id) {}
    };

    class SourceNode : public Node, public ProductSource
    {
    public:
        using Ptr = std::shared_ptr<SourceNode>;

    private:
        const size_t _processTime;
        size_t _currentProcessTime;

        SourceLinksHub _sourceLinksHub;

    public:
        SourceNode() = delete;
        SourceNode(size_t id, size_t processTime) : Node(id), _processTime(processTime) {}
        SourceNode(const SourceNode &) = delete;
        SourceNode(SourceNode &&) = delete;

        SourceNode &operator=(const SourceNode &) = delete;
        SourceNode &operator=(SourceNode &&) = delete;

        size_t getProcessTime() const { return _processTime; }

        void proceed(const size_t currentTime)
        {
            if (++_currentProcessTime >= _processTime)
            {
                auto ptr = moveOut();
                if(ptr) {
                    _sourceLinksHub.passProduct(std::move(ptr));
                }
                reset();
            }
        }

        void reset() { _currentProcessTime = 0;}

        SourceLinksHub &getSourceLinksHub() { return _sourceLinksHub; }
    };

    class SinkNode : public Node, public ProductSink
    {
    public:
        using Ptr = std::shared_ptr<SinkNode>;

    private:
        std::deque<Product::Ptr> _storedProducts;
        SinkLinksHub _sinkLinksHub;

    public:
        SinkNode() = delete;
        SinkNode(size_t id) : Node(id) {}
        SinkNode(const SinkNode &) = delete;
        SinkNode(SinkNode &&) = delete;

        SinkNode &operator=(const SinkNode &) = delete;
        SinkNode &operator=(SinkNode &&) = delete;

        void moveIn(Product::Ptr &&product) override
        {
            _storedProducts.emplace_back(std::move(product));
        }

        SinkLinksHub &getSinkLinksHub() { return _sinkLinksHub; }

        bool empty() { return _storedProducts.empty(); }

        Product::Ptr getProduct( bool first = false) {
            if(empty()) {
                throw std::runtime_error("Sink is empty");
            }
            Product::Ptr result;
            if(first) {
                result = std::move(_storedProducts.front());
                _storedProducts.pop_front();
            } else {
                result = std::move(_storedProducts.back());
                _storedProducts.pop_back();
            }
            return std::move(result);
        }
    };
}