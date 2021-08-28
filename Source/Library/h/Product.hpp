#pragma once
#include <memory>
#include "Identifiable.hpp"

namespace sd
{
    class Product : public Identifiable
    {
    private:
        static size_t _idSeed;

    public:
        using Ptr = std::unique_ptr<Product>;

        Product() : Identifiable(_idSeed++) {}
    };
}