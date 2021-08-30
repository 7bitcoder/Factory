#pragma once
#include <iostream>
#include <filesystem>

#include "Net.hpp"
#include "Interfaces.hpp"

namespace sd
{
    class Factory
    {
    private:
        Configuration _config;
        Net _net;
        std::unique_ptr<CLI::App> _cli;

    public:
        Factory(Configuration && config);
        ~Factory();

        void load(const std::filesystem::path &file);
        
        void run();

        void build();
    private:
        void buildCli();
        void loadNet(std::istream& str);
        void createLink(const std::vector<std::string>& input);
        void createWorker(const std::vector<std::string>& input);
        void createLoadingRamp(const std::vector<std::string>& input);
        void createStoreHause(const std::vector<std::string>& input);
    };
}