/**
    @file   elapsed_timer.cpp
    @author kuumies <kuumies@gmail.com>
    @brief  Implementation of kuu::ElapsedTimer class.
 **/

#include "elapsed_timer.h"

namespace kuu
{

ElapsedTimer::ElapsedTimer()
{
    prevTime_ = Clock::now();
}

int ElapsedTimer::elapsed()
{

    ClockTimePoint currentTime = Clock::now();
    auto diffTime = currentTime - prevTime_;
    prevTime_ = currentTime;

    using namespace std::chrono;
    return duration_cast<milliseconds>(diffTime).count();
}

} // namespace kuu
