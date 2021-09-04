#include "Processable.hpp"

namespace sd
{
    Processable::Processable(size_t processTime) : _totalProcessTime(processTime), _currentProcessTime(0), _stopped(false) {}

    size_t Processable::getTotalProcesingTime() const { return _totalProcessTime; }

    size_t Processable::getCurrentProcesingTime() const { return _currentProcessTime; }

    void Processable::process(const size_t currentTime)
    {
        if (!_stopped && (++_currentProcessTime >= _totalProcessTime))
        {
            triggerOperation();
            resetProcessTime();
        }
    }

    void Processable::stop()
    {
        _stopped = true;
        resetProcessTime();
    }

    void Processable::reset()
    {
        _stopped = false;
        resetProcessTime();
    }

    void Processable::resetProcessTime() { _currentProcessTime = 0; }
}