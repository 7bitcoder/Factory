#include <memory>
#include <iostream>

#include "CLI11.hpp"
#include "CommandParser.hpp"

namespace sd
{

    namespace
    {
        const std::string help =
            R"help(Argument Error, 
        describtion
        first argument: resolution width - number (optional)
        second argument: resolution height - number (optional)
        third argument: video mode - number (optional):
        posible values: None = 0, Titlebar = 1, Resize = 2, Close = 4, Fullscreen = 8, values can be mixed with bitwise operators
        example: 1920 1080 8)help";
        int maxArguments = 3;
    }

    CommandParser::CommandParser()
    {
        _app = std::make_unique<CLI::App>("Factory");
        
        _app->add_option<decltype(_results.stateRaportTimings), std::vector<size_t>>(
            "-rt,--raportTimings",
            _results.stateRaportTimings,
            "State raport will be generater every iterations provided in list");

        _app->add_option<decltype(_results.stateRaportTimings), size_t>(
            "-ri,--raportInterval",
            _results.stateRaportTimings,
            "State raport will be generater every interval");

        _app->add_option(
            "-rf,--raportFile",
            _results.raportFile,
            "State raports will be saved in this file");

        _app->add_option(
            "-i,--maxIterations",
            _results.maxIterations,
            "Maximum iteration that simulation will be run");

        _app->add_option(
            "-f,--file",
            _results.structureFile,
            "File that contains fabric structure");

        _app->add_flag(
            "-sr,--showStructureRaports",
            _results.showStructureRaport,
            "On every net modifications structure raport will be shown");
    }

    bool CommandParser::parse(int argc, char **argv)
    {
        try
        {
            _app->parse(argc, argv);
        }
        catch (const CLI::ParseError &e)
        {
            _app->exit(e);
            return false;
        }
        return true;
    }
}