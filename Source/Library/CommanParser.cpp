#include <memory>
#include <iostream>

#include "CLI11.hpp"
#include "CommandParser.hpp"

namespace sd
{

    CommandParser::~CommandParser() {}

    CommandParser::CommandParser()
    {
        _app = std::make_unique<CLI::App>("Factory");

        _app->add_option<decltype(_results.stateRaportTimings), std::vector<size_t>>(
            "-t,--raportTimings",
            _results.stateRaportTimings,
            "State raport will be generater every iterations provided in list");

        _app->add_option<decltype(_results.stateRaportTimings), size_t>(
            "-i,--raportInterval",
            _results.stateRaportTimings,
            "State raport will be generater every interval");

        _app->add_option(
            "-r,--raportFile",
            _results.raportFile,
            "State raports will be saved in this file");

        _app->add_option(
            "-m,--maxIterations",
            _results.maxIterations,
            "Maximum iteration that simulation will be run");

        auto file = _app->add_option(
            "-f,--file",
            _results.structureFile,
            "File that contains fabric structure");
        file->check(CLI::ExistingFile);
        file->required();

        _app->add_flag(
            "-s,--showStructureRaports",
            _results.showStructureRaport,
            "On every net modifications structure raport will be shown");
    }

    bool CommandParser::parse(int argc, char **argv, std::ostream &out, std::ostream &err)
    {
        try
        {
            _app->parse(argc, argv);
        }
        catch (const CLI::ParseError &e)
        {
            _app->exit(e, out, err);
            return false;
        }
        return true;
    }

    const Configuration &CommandParser::getResults() const { return _results; }

}