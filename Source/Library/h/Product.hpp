#pragma once

#include <memory>

#include "Interfaces.hpp"
#include "Identifiable.hpp"

namespace sd
{
    class Product final: public Identifiable, public ToString
    {
    private:
        static size_t _idSeed;

    public:
        using Ptr = std::unique_ptr<Product>;

        std::string toString() final;

        Product();
    };
}