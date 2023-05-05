// freqreq.cpp

#include <iostream>
#include <string>
#include "frequencyLimiter.h"

// unit test main
int main(int argc, char *argv[])
{
    // locals
    uint32_t PCoreACCoreFrequency;
    uint32_t PCoreDCCoreFrequency;
    uint32_t ECoreACCoreFrequency;
    uint32_t ECoreDCCoreFrequency;
    frequencyLimiter FrequencyLimiter;
    bool IsHybrid;
    int Ret;

    /*
        int PCoreACCoreFrequency, PCoreDCCoreFrequency, ECoreACCoreFrequency, ECoreDCCoreFrequency;
        std::cout << "Frequency Limiter Unit Test:" << std::endl;
        if (argc != 5) {
         std::cout << "usage: freqreq PCoreACMaxFreq PCoreDCMaxFreq ECoreACMaxFreq EcoreDCMaxFreq " << "(use 0 default and for ECore non-hybrid systems)" << std::endl;
         return ERROR_INPUT;
        }
        PCoreACCoreFrequency = std::stoi(argv[1]);
        PCoreDCCoreFrequency = std::stoi(argv[2]);
        ECoreACCoreFrequency = std::stoi(argv[3]);
        ECoreDCCoreFrequency = std::stoi(argv[4]);
    */

    // check for hybrid
    Ret = FrequencyLimiter.IsHybridCore(IsHybrid);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    Ret = FrequencyLimiter.SetUpStepping(1);
    Ret = FrequencyLimiter.SetDownStepping(MAX_DOWN_STEPPING);

    // this checks scaling
    FrequencyLimiter.GearUp(3);
    FrequencyLimiter.GearDown(3);
    FrequencyLimiter.GearUp(3);
    FrequencyLimiter.GearDown(1);

    //    Ret = FrequencyLimiter.SetCoreMaxFrequency(PCORE, 0, 0);
    //    Ret = FrequencyLimiter.SetCoreMaxFrequency(ECORE, 0, 0);


    // output pcore ac dc frequencies
    Ret = FrequencyLimiter.GetCoreMaxFrequency(PCORE, PCoreACCoreFrequency, PCoreDCCoreFrequency);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    std::cout << "PCore AC Freq " << PCoreACCoreFrequency << std::endl
              << "PCore DC Freq " << PCoreDCCoreFrequency << std::endl;
    if (IsHybrid) {

        Ret = FrequencyLimiter.GetCoreMaxFrequency(ECORE, ECoreACCoreFrequency, ECoreDCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        // output ecore ac dc frequencies
        std::cout << "ECore AC Freq " << ECoreACCoreFrequency << std::endl
            << "ECore DC Freq " << ECoreDCCoreFrequency << std::endl;
    }
 /*
    // if hybrid system
    if (IsHybrid)
    {
        //
        // get ecore ac dc frequency
        Ret = FrequencyLimiter.GetCoreMaxFrequency(ECORE, ACCoreFrequency, DCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        //

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
    */
GracefulExit:
    std::cout << std::endl
              << "Return Code " << Ret << std::endl;
    return Ret;
}
