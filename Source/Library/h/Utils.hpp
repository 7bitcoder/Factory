#pragma once

#include <iostream>
#include <vector>
#include <memory>

namespace sd
{
    class Factory;

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

    std::vector<std::string> splitStr(const std::string &str, char splitChar);

    std::ostream &operator<<(std::ostream &stream, const Factory &factory);
    std::istream &operator>>(std::istream &stream, Factory &factory);

    std::string getOffset(size_t offset);

    std::string toString(NodeType type);
    std::string toString(WorkerType type);
}