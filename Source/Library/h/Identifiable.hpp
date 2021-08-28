#pragma once
namespace sd
{
    class Identifiable
    {
    private:
        const size_t _id;

    public:
        Identifiable(size_t id) : _id(id) {}

    public:
        size_t getId() const { return _id; }
    };
}