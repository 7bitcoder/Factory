#pragma once
#include <iostream>
#include <filesystem>

#include "Factory.hpp"
#include "Interfaces.hpp"

namespace sd
{
    class Controller
    {
    private:
        Factory::Ptr _factory;
        Configuration _config;
        std::unique_ptr<CLI::App> _cli;
        std::ostream &_out;
        std::ostream &_err;
        std::istream &_in;

    public:
        Controller(Configuration &&config);
        ~Controller();

        void run();
    private:
        std::ostream &getOut();
        std::ostream &getErr();
        std::istream &getIn();
        Factory::Ptr createFactory(const std::optional<std::string> &filePath);
        Factory::Ptr createFactoryFromFile(const std::filesystem::path &filePath);
        void saveFactoryToFile(const std::filesystem::path &file) const;
        void runSimulation(const std::optional<std::string> &raportfilePath, size_t maxIterations, const Factory::RaportGuard &raportGuard);
        void buildCommandLineInterface();
    };
}