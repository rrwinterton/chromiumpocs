// freqreq.cpp

#include <iostream>
#include <string>
#include "frequencyLimiter.h"

// unit test main
int main(int argc, char* argv[])
{
    // locals
    frequencyLimiter FrequencyLimiter;
    int ACCoreFrequency, DCCoreFrequency;
    bool IsHybrid;
    int Ret;

    std::cout << "Frequency Limiter Unit Test:" << std::endl;
    if (argc != 3) {
     std::cout << "usage: freqreq ACMaxFreq DCMaxFreq" << std::endl;
     return ERROR_INPUT;
    }

    ACCoreFrequency = std::stoi(argv[1]);
    DCCoreFrequency = std::stoi(argv[2]);

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
    Ret = FrequencyLimiter.SetCoreMaxFrequency(PCORE, ACCoreFrequency, DCCoreFrequency);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    // verify you set pcore ac dc frequency
    Ret = FrequencyLimiter.GetCoreMaxFrequency(PCORE, ACCoreFrequency, DCCoreFrequency);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    // output pcore ac dc frequency
    std::cout << "PCore AC Freq " << ACCoreFrequency << std::endl
              << "PCore DC Freq " << DCCoreFrequency << std::endl;

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
        Ret = FrequencyLimiter.SetCoreMaxFrequency(ECORE, ACCoreFrequency, DCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        // verify ecore ac dc frequency
        Ret = FrequencyLimiter.GetCoreMaxFrequency(ECORE, ACCoreFrequency, DCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        std::cout << "ECore AC Freq " << ACCoreFrequency << std::endl
                  << "ECore DC Freq " << DCCoreFrequency << std::endl;
    }
GracefulExit:
    std::cout << std::endl
              << "Return Code " << Ret << std::endl;
    return Ret;
}
