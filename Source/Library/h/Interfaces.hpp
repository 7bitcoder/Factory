#pragma once

#include <string>
#include <variant>
#include <vector>
#include <optional>

namespace CLI
{
    class App;
}

namespace sd
{
    struct Configuration
    {
        std::optional<std::string> structureFile = std::nullopt;
        size_t maxIterations = 100;

        std::variant<size_t, std::vector<size_t>> stateRaportTimings = size_t{20};
        bool showStructureRaport = false;
        std::optional<std::string> raportFile = std::nullopt;
    };

    enum NodeType
    {
        RAMP,
        WORKER,
        STORE
    };

    enum WorkerType
    {
        LIFO,
        FIFO
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

    inline std::string toString(WorkerType type)
    {
        return type == WorkerType::FIFO ? "FIFO" : "LIFO";
    }
}