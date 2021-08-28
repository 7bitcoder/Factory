#include "Factory.hpp"

namespace sd
{

    void Factory::load(const std::filesystem::path &file) {}

    void Factory::build()
    {
        _net.build();
    }

}