#pragma once
#include <iostream>
#include <memory>

#include "Interfaces.hpp"

namespace sd
{
    class CommandParser
    {
    private:
        std::unique_ptr<CLI::App> _app;
        Configuration _results;

    public:
        CommandParser();
        ~CommandParser();

        bool parse(int argc, char **argv);
        Configuration getResults() { return _results; }
    };
}