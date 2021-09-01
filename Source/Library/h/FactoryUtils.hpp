#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include "factory.hpp"

namespace sd
{
    class Factory;

    std::ostream &operator<<(std::ostream &stream, const Factory &factory);
    std::istream &operator>>(std::istream &stream, Factory &factory);
}