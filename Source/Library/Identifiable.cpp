#include "Identifiable.hpp"

namespace sd
{
    Identifiable::Identifiable(size_t id) : _id(id)
    {
    }

    size_t Identifiable::getId() const
    {
        return _id;
    }
} // namespace sd