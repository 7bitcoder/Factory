#include "Random.hpp"

namespace sd
{
    NormalRandomDevice::NormalRandomDevice() : _eng(_rd()), _distr(0, 1) {}

    double NormalRandomDevice::next() { return _distr(_eng); }

    Random::Random(std::unique_ptr<IRandomDevice> newRandomDevice)
    {
        updateRandomDevice(std::move(newRandomDevice));
    }

    Random &Random::get()
    {
        static Random r{std::make_unique<NormalRandomDevice>()};
        return r;
    }

    double Random::next() { return _randomDevice->next(); }

    void Random::updateRandomDevice(std::unique_ptr<IRandomDevice> newRandomDevice)
    {
        _randomDevice = std::move(newRandomDevice);
    }
}