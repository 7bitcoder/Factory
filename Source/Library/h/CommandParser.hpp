#pragma once
#include <iostream>
#include <memory>
#include <variant>

namespace sd
{
    class CommandParser
    {
    public:
        struct Results
        {
            std::string structureFile;
            size_t maxIterations;
            
            std::variant<size_t, std::vector<size_t>> stateRaportTimings;
            bool showStructureRaport;
            std::string raportFile;
        };

        CommandParser();
        ~CommandParser() = default;

        bool parse(int argc, char **argv);
        Results &getResults() { return _results; }

    private:
        std::unique_ptr<CLI::App> _app;
        Results _results;
    };
}