#pragma once

#include <string>
#include <memory>

#include "Utils.hpp"

namespace sd
{
    class Product;

    struct IRandomDevice {
        virtual double next() = 0;
    };

    struct IProductSource
    {
        virtual std::unique_ptr<Product> moveOutProduct() = 0;
    };

    struct IProductDestination
    {
        virtual void moveInProduct(std::unique_ptr<Product> &&product) = 0;
    };

    struct IToString
    {
        virtual std::string toString() const = 0;
    };

    struct IType
    {
        virtual NodeType getNodeType() const = 0;
    };

    struct IStructureRaportable
    {
        virtual std::string getStructureRaport(size_t offset) const = 0;
    };

    struct IStateRaportable
    {
        virtual std::string getStateRaport(size_t offset) const = 0;
    };
}