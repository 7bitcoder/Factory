#pragma once

#include <memory>

#include "Identifiable.hpp"
#include "Interfaces.hpp"


namespace sd
{
    class Product final : public Identifiable, public IToString
    {
      private:
        static size_t _idSeed;

      public:
        using Ptr = std::unique_ptr<Product>;

        Product();

        std::string toString() const final;
    };
} // namespace sd