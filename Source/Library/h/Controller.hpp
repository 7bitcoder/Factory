#pragma once
#include <iostream>
#include <filesystem>

#include "Factory.hpp"
#include "Interfaces.hpp"
#include "Configuration.hpp"

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
        Controller(const Configuration &config, std::ostream &out, std::ostream &err, std::istream &in);
        ~Controller();

        void run();

    private:
        void buildCommandLineInterface();

        void runSimulation(const std::optional<std::string> &raportfilePath, size_t maxIterations, const Factory::RaportGuard &raportGuard);

        std::ostream &getOut();
        std::ostream &getErr();
        std::istream &getIn();
    };
}