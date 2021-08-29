#pragma once
#include <iostream>
#include <filesystem>

#include "Net.hpp"

namespace sd
{
    class Factory
    {
    private:
        Net _net;

    public:

        void load(const std::filesystem::path &file);
        
        void run();

        void build();
    private:
        void loadNet(std::istream& str);
        void createLink(const std::vector<std::string>& input);
        void createWorker(const std::vector<std::string>& input);
        void createLoadingRamp(const std::vector<std::string>& input);
        void createStoreHause(const std::vector<std::string>& input);
    };
}