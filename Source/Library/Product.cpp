#pragma once
#include "Product.hpp"

namespace sd
{
    size_t Product::_idSeed = 0;

    Product::Product() : Identifiable(_idSeed++) {}

}