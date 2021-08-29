#pragma once

#include <format>

#include "Product.hpp"

namespace sd
{
    size_t Product::_idSeed = 0;

    Product::Product() : Identifiable(_idSeed++) {}

    std::string Product::toString() { return std::format("#{}", getId()); }
}