#pragma once

#include <random>
#include <memory>

#include "Interfaces.hpp"

namespace sd {
    class NormalRandomDevice final : public IRandomDevice
    {
    private:
        std::random_device _rd;
        std::default_random_engine _eng;
        std::uniform_real_distribution<double> _distr;
    public:
        NormalRandomDevice();
        double next() final;
    };

    class Random final : public IRandomDevice
    {
    private:
        std::unique_ptr<IRandomDevice> _randomDevice;

        Random(std::unique_ptr<IRandomDevice> newRandomDevice);
    public:
        static Random& get();

        void updateRandomDevice(std::unique_ptr<IRandomDevice> newRandomDevice);
        double next() final;
    };
}