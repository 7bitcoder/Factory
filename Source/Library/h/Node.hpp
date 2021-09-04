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
          virtual public IStructureRaportable
    {
    private:
        Product::Ptr _product;

        std::vector<Link::Ptr> _links;
    public:

        using Ptr = std::shared_ptr<SourceNode>;
        using RawPtr = SourceNode *;

        SourceNode(size_t id);

        void setProduct(Product::Ptr &&product);

        void passProduct();

        std::string getStructureRaport(size_t offset) const override;

        void bindSourceLink(Link::Ptr link);
        void unBindSourceLink(size_t id);

        bool connectedSources() const;

        void unbindAllSources();

        bool isProductReady() const;
    private:
        void normalize();

        Link::Ptr getRandomLink() const;
    };

    class DestinationNode
        : virtual public Node,
          virtual public IStructureRaportable,
          public IStateRaportable
    {
    private:
        std::deque<Product::Ptr> _storedProducts;

        std::vector<Link::Ptr> _links;

    public:
        using Ptr = std::shared_ptr<DestinationNode>;
        using RawPtr = DestinationNode *;

        DestinationNode(size_t id);

        void addProductToStore(Product::Ptr &&product);

        Product::Ptr getStoredProduct(bool first = false);

        std::string getStateRaport(size_t offset) const override;

        void bindDestinationLink(Link::Ptr link);
        void unBindDestinationLink(size_t id);

        bool connectedDestinations() const;

        void unbindAllDestinations();

        bool areProductsAvailable() const;
        size_t getStoredProductsSize() const;
    };
}