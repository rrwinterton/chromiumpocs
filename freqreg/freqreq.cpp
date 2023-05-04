// freqreq.cpp

#include <iostream>
#include <string>
#include "frequencyLimiter.h"

// unit test main
int main(int argc, char* argv[])
{
    // locals
    frequencyLimiter FrequencyLimiter;
    int PCoreACCoreFrequency, PCoreDCCoreFrequency, ECoreACCoreFrequency, ECoreDCCoreFrequency;
    bool IsHybrid;
    int Ret;

    std::cout << "Frequency Limiter Unit Test:" << std::endl;
    if (argc != 5) {
     std::cout << "usage: freqreq PCoreACMaxFreq PCoreDCMaxFreq ECoreACMaxFreq EcoreDCMaxFreq " << "(use 0 default and for ECore non-hybrid systems)" << std::endl;
     return ERROR_INPUT;
    }

    PCoreACCoreFrequency = std::stoi(argv[1]);
    PCoreDCCoreFrequency = std::stoi(argv[2]);
    ECoreACCoreFrequency = std::stoi(argv[3]);
    ECoreDCCoreFrequency = std::stoi(argv[4]);

    // check for hybrid
    Ret = FrequencyLimiter.IsHybridCore(IsHybrid);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    /*
    // get pcore ac dc frequency
    Ret = FrequencyLimiter.GetCoreMaxFrequency(PCORE, ACCoreFrequency, DCCoreFrequency);
    if (Ret != 0)
    {
        goto GracefulExit;
    }
    */

    // set pcore ac dc frequency
    Ret = FrequencyLimiter.SetCoreMaxFrequency(PCORE, PCoreACCoreFrequency, PCoreDCCoreFrequency);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    // verify you set pcore ac dc frequency
    Ret = FrequencyLimiter.GetCoreMaxFrequency(PCORE, PCoreACCoreFrequency, PCoreDCCoreFrequency);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    // output pcore ac dc frequency
    std::cout << "PCore AC Freq " << PCoreACCoreFrequency << std::endl
              << "PCore DC Freq " << PCoreDCCoreFrequency << std::endl;

    // if hybrid system
    if (IsHybrid)
    {
        /*
        // get ecore ac dc frequency
        Ret = FrequencyLimiter.GetCoreMaxFrequency(ECORE, ACCoreFrequency, DCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        */

        // set ecore ac dc frequency
        Ret = FrequencyLimiter.SetCoreMaxFrequency(ECORE, ECoreACCoreFrequency, ECoreDCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        // verify ecore ac dc frequency
        Ret = FrequencyLimiter.GetCoreMaxFrequency(ECORE, ECoreACCoreFrequency, ECoreDCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        std::cout << "ECore AC Freq " << ECoreACCoreFrequency << std::endl
                  << "ECore DC Freq " << ECoreDCCoreFrequency << std::endl;
    }
GracefulExit:
     std::cout << std::endl
              << "Return Code " << Ret << std::endl;
    return Ret;
}
