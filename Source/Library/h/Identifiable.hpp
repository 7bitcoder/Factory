#pragma once
namespace sd
{
    class Identifiable
    {
    private:
        const size_t _id;

    public:
        Identifiable(size_t id);

        size_t getId() const;
    };
}