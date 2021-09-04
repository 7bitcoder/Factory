#pragma once

inline bool operator==(const sd::LoadingRampData &lhs, const sd::LoadingRampData &rhs)
{
    return lhs.id == rhs.id && lhs.deliveryInterval == rhs.deliveryInterval;
}

inline bool operator==(const sd::WorkerData &lhs, const sd::WorkerData &rhs)
{
    return lhs.id == rhs.id && lhs.processingTime == rhs.processingTime && lhs.type == rhs.type;
}

inline bool operator==(const sd::StoreHouseData &lhs, const sd::StoreHouseData &rhs)
{
    return lhs.id == rhs.id;
}

inline bool operator==(const sd::LinkData &lhs, const sd::LinkData &rhs)
{
    return lhs.id == rhs.id && lhs.probability == rhs.probability &&
           lhs.source.id == rhs.source.id && lhs.source.type == rhs.source.type &&
           lhs.destination.id == rhs.destination.id && lhs.destination.type == rhs.destination.type;
}

class RepetableRandomDevice final : public sd::IRandomDevice
{
private:
    std::default_random_engine _eng;
    std::uniform_real_distribution<double> _distr;

public:
    RepetableRandomDevice() : _distr(0, 1) {}
    double next() final { return _distr(_eng); }
};

template <class T>
inline bool cmp(const std::vector<T> &left, const std::vector<T> &right)
{
    if (left.size() != right.size())
    {
        return false;
    }
    for (int i = 0; i < left.size(); ++i)
    {
        if (!(left[i] == right[i]))
        {
            return false;
        }
    }
    return true;
}