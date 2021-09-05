#pragma once

#include <memory>
#include <string>


#include "Utils.hpp"

namespace sd
{
    class Product;

    struct IRandomDevice
    {
        virtual double next() = 0;

        virtual ~IRandomDevice()
        {
        }
    };

    struct IToString
    {
        virtual std::string toString() const = 0;

        virtual ~IToString()
        {
        }
    };

    struct IType
    {
        virtual NodeType getNodeType() const = 0;

        virtual ~IType()
        {
        }
    };

    struct IProcessable
    {
        virtual void process(const size_t currentTime) = 0;

        virtual ~IProcessable()
        {
        }

      protected:
        virtual void triggerOperation() = 0;
    };

    struct IStructureRaportable
    {
        virtual std::string getStructureRaport(size_t offset) const = 0;

        virtual ~IStructureRaportable()
        {
        }
    };

    struct IStateRaportable
    {
        virtual std::string getStateRaport(size_t offset) const = 0;

        virtual ~IStateRaportable()
        {
        }
    };
} // namespace sd