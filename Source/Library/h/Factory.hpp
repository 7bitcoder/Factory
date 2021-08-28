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

        void build();
    };
}