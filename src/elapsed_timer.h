/**
    @file   elapsed_timer.h
    @author kuumies <kuumies@gmail.com>
    @brief  Definition of kuu::ElapsedTimer class.
 **/

#pragma once

#include <chrono>

namespace kuu
{

/**
    A simple timer for getting the elapsed time in milliseconds.
 **/
class ElapsedTimer
{
public:
    using Clock = std::chrono::steady_clock;
    using ClockTimePoint = Clock::time_point;

    /**
        Constructs the elapsed timer
     **/
    ElapsedTimer();

    /**
        Returns the elapsed time in milliseconds since the function
        was previously called.
        @return The elapsed time in milliseconds.
     **/
    int elapsed();

private:
    ClockTimePoint prevTime_; // previous sampling time
};

} // namespace kuu
