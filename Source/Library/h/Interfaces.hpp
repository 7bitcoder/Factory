#pragma once

#include <string>
#include <variant>
#include <vector>

namespace CLI
{
    class App;
}

namespace sd
{
    struct Configuration
    {
        std::string structureFile = "";
        size_t maxIterations = 100;

        std::variant<size_t, std::vector<size_t>> stateRaportTimings = size_t{0};
        bool showStructureRaport = false;
        std::string raportFile = "";
    };

    enum NodeType
    {
        RAMP,
        WORKER,
        STORE
    };

    struct ToString
    {
        virtual std::string toString() = 0;
    };

    struct Type
    {
        virtual NodeType getNodeType() = 0;
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

    inline std::string toString(NodeType type)
    {
        switch (type)
        {
        case NodeType::WORKER:
            return "worker";
        case NodeType::RAMP:
            return "ramp";
        case NodeType::STORE:
            return "store";
        default:
            return "";
        }
    }
}