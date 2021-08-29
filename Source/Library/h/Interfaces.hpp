#pragma once

#include <string>

namespace sd
{
    struct ToString
    {
        virtual std::string toString() = 0;
    };

    struct Structure
    {
        virtual std::string getStructure() = 0;
    };

    struct StructureRaportable
    {
        virtual std::string getStructureRaport(size_t offset) = 0;
    };

    struct StateRaportable
    {
        virtual std::string getStateRaport(size_t offset) = 0;
    };

    inline std::string getOffset(size_t offset) { return std::string(offset, '\t'); }
}