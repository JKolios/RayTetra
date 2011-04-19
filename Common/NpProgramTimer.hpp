#ifndef _NpProgramTimer_h_
#define _NpProgramTimer_h_


#include <ctime>
#include <string>
#include <iostream>


class NpProgramTimer  
{
public:
    NpProgramTimer();
    NpProgramTimer(std::string timerName);

    void  Start();
    void  Stop();
    
    std::string Name() const;
    
    clock_t ElapsedTime() const;
    clock_t MeanElapsedTime() const;
    clock_t TotalElapsedTime() const;
    
    void  ResetMean();

private:
    std::string mName;
    clock_t mStart;
    clock_t mStop;
    int     mNumTimings;
    clock_t mTotalTime;
};

std::ostream& operator<<(std::ostream& ostr, const NpProgramTimer& timer);



// Inline functions.


inline std::ostream& operator<<(std::ostream& ostr, const NpProgramTimer& timer)
{
    ostr << "Timer <" << timer.Name() << ">";

    return ostr;
}


#endif // _NpProgramTimer_h_
