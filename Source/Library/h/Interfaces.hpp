#pragma once

#include <string>
#include <memory>

#include "Utils.hpp"

namespace sd
{
    class Product;

    struct ProductSource
    {
        virtual std::unique_ptr<Product> moveOutProduct() = 0;
    };

    struct ProductDestination
    {
        virtual void moveInProduct(std::unique_ptr<Product> &&product) = 0;
    };

    struct ToString
    {
        virtual std::string toString() const = 0;
    };

    struct Type
    {
        virtual NodeType getNodeType() const = 0;
    };

    struct StructureRaportable
    {
        virtual std::string getStructureRaport(size_t offset) const = 0;
    };

    struct StateRaportable
    {
        virtual std::string getStateRaport(size_t offset) const = 0;
    };
}