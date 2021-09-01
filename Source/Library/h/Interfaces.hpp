#pragma once

#include <string>

#include "Utils.hpp"

namespace sd
{
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