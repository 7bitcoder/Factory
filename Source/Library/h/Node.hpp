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
        virtual Product::Ptr moveOutProduct() = 0;
    };

    struct ProductSink
    {
        virtual void moveInProduct(Product::Ptr &&product) = 0;
    };

    class Node : public Identifiable
    {
    public:
        using Ptr = std::shared_ptr<Node>;
        Node(size_t id);
    };

    class SourceNode : public Node, public ProductSource
    {
    public:
        using Ptr = std::shared_ptr<SourceNode>;

    private:
        const size_t _processTime = 0;
        size_t _currentProcessTime = 0;

        SourceLinksHub _sourceLinksHub;

    public:
        SourceNode() = delete;
        SourceNode(size_t id, size_t processTime);
        SourceNode(const SourceNode &) = delete;
        SourceNode(SourceNode &&) = delete;

        SourceNode &operator=(const SourceNode &) = delete;
        SourceNode &operator=(SourceNode &&) = delete;

        size_t getProcessTime() const;

        void process(const size_t currentTime);

        void resetProcessTime();

        SourceLinksHub &getSourceLinksHub();
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
        SinkNode(size_t id);
        SinkNode(const SinkNode &) = delete;
        SinkNode(SinkNode &&) = delete;

        SinkNode &operator=(const SinkNode &) = delete;
        SinkNode &operator=(SinkNode &&) = delete;

        void moveInProduct(Product::Ptr &&product) override;

        SinkLinksHub &getSinkLinksHub();

        bool areProductsAvailable() const;

        Product::Ptr getProduct(bool first = false);
    };
}