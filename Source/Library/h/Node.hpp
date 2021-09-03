#pragma once
#include <memory>
#include <deque>

#include "Interfaces.hpp"
#include "Identifiable.hpp"
#include "Product.hpp"
#include "Link.hpp"

namespace sd
{
    class Node
        : public Identifiable,
          public IToString,
          public IType
    {
    public:
        using Ptr = std::shared_ptr<Node>;
        Node(size_t id);
    };

    class SourceNode
        : virtual public Node,
          virtual public IStructureRaportable,
          public IProductSource
    {
    private:
        const size_t _processTime = 0;
        size_t _currentProcessTime = 0;

        std::vector<Link::Ptr> _links;
    public:

        using Ptr = std::shared_ptr<SourceNode>;
        using RawPtr = SourceNode *;

        SourceNode(size_t id, size_t processTime);

        size_t getProcesingTime() const;
        size_t getCurrentProcesingTime() const;

        void process(const size_t currentTime);

        void resetProcessTime();

        void bindSourceLink(Link::Ptr link);
        void unBindSourceLink(size_t id);

        void passProduct(Product::Ptr &&product);

        std::string getStructureRaport(size_t offset) const override;

        bool connectedSources() const;

        void unbindAllSources();

    private:
        void normalize();

        Link::Ptr getRandomLink() const;
    };

    class DestinationNode
        : virtual public Node,
          virtual public IStructureRaportable,
          public IStateRaportable,
          public IProductDestination
    {
    private:
        std::deque<Product::Ptr> _storedProducts;

        std::vector<Link::Ptr> _links;

    public:
        using Ptr = std::shared_ptr<DestinationNode>;
        using RawPtr = DestinationNode *;

        DestinationNode(size_t id);

        void moveInProduct(Product::Ptr &&product) override;

        std::string getStoredProductsRaport() const;

        bool areProductsAvailable() const;

        Product::Ptr getProduct(bool first = false);

        void bindDestinationLink(Link::Ptr link);
        void unBindDestinationLink(size_t id);

        bool connectedDestinations() const;

        void unbindAllDestinations();

        size_t getStoredProducts() const;
    };
}