#pragma once

#include "Interfaces.hpp"

namespace sd
{

    class Processable : public IProcessable
    {
    private:
        const size_t _totalProcessTime;
        size_t _currentProcessTime;
        bool _stopped;

    public:
        Processable(size_t processTime);

        void process(const size_t currentTime) override;

    protected:
        size_t getTotalProcesingTime() const;

        size_t getCurrentProcesingTime() const;

        void stop();

        void reset();

    private:
        void resetProcessTime();
    };
}