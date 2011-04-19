#include "NpProgramTimer.hpp"

#include <cstring>
#include <iostream>


NpProgramTimer::NpProgramTimer()
    : mName("Timer"), mStart(clock_t(-1)), mStop(clock_t(-1)), 
      mNumTimings(0), mTotalTime(0)
{
}


NpProgramTimer::NpProgramTimer(std::string timerName)
    : mName(timerName), mStart(clock_t(-1)), mStop(clock_t(-1)), 
      mNumTimings(0), mTotalTime(0)
{
}


void NpProgramTimer::Start()
{
    mStart = clock();

    if (mStart == clock_t(-1))
        std::cerr << *this << " Error: Could not start timer." << std::endl;
}


void NpProgramTimer::Stop()
{
    mStop = clock();

    if (mStop == clock_t(-1))
        std::cerr << *this << " Error: Could not stop timer." << std::endl;
    else  {
        ++mNumTimings;
        mTotalTime += ElapsedTime();
    }
}


std::string NpProgramTimer::Name() const
{
    return mName;
}


clock_t NpProgramTimer::ElapsedTime() const
{
    if ((mStart == clock_t(-1))  ||  (mStop == clock_t(-1)))  {
        std::cerr << *this << " Error: Invalid start/stop." << std::endl;
        
        return clock_t(-1);
    }
    else  {
        return (clock_t(mStop-mStart) / (CLOCKS_PER_SEC / 1000));
    }
}


clock_t NpProgramTimer::MeanElapsedTime() const
{
    if (mNumTimings == 0)  {
        std::cerr << *this << " Error: No timings made so far." << std::endl;
        return clock_t(-1);
    }
    else  {
        return (mTotalTime / mNumTimings);
    }
}


clock_t NpProgramTimer::TotalElapsedTime() const
{
    if (mNumTimings == 0)  {
        std::cerr << *this << " Error: No timings made so far." << std::endl;
        return clock_t(-1);
    }
    else  {
        return mTotalTime;
    }
}


void NpProgramTimer::ResetMean()
{
    mTotalTime = 0;
    mNumTimings = 0;
}

