// freqreq.cpp

#include <iostream>
#include <string>
#include "frequencyLimiter.h"

// unit test main
int main(int argc, char *argv[])
{
    // locals
    uint32_t gearupValue;
    uint32_t geardownValue;
    frequencyLimiter FrequencyLimiter;
    bool IsHybrid;
    uint32_t CurrentFrequency;
    int Ret;

    uint32_t PCoreACCoreFrequency;
    uint32_t PCoreDCCoreFrequency;
    uint32_t ECoreACCoreFrequency;
    uint32_t ECoreDCCoreFrequency;

    std::cout << "Frequency Regulating Unit Tests:" << std::endl;
    if (argc != 3)
    {
        std::cout << "usage: freqreq geardownValue gearupValue "
                  << "(Max number of knobs for now 100, use 0-100 for now)" << std::endl;
        return ERROR_INPUT;
    }
    geardownValue = std::stoi(argv[1]);
    gearupValue = std::stoi(argv[2]);

    // check for hybrid
    Ret = FrequencyLimiter.IsHybridCore(IsHybrid);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    Ret = FrequencyLimiter.CalculateFrequency(CurrentFrequency);
    std::cout << "calculated Frequency " << CurrentFrequency << std::endl;

    //    Ret = FrequencyLimiter.SetSteppingKnobs(MAX_KNOBS); //needs to change to scale steppings now one to one

    // geardown test
    FrequencyLimiter.GearDown(geardownValue);

    // output pcore ac dc frequencies
    Ret = FrequencyLimiter.GetCoreMaxFrequency(PCORE, PCoreACCoreFrequency, PCoreDCCoreFrequency);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    std::cout << std::endl
              << "Test Gear Down " << geardownValue << std::endl;
    std::cout << "PCore AC Freq " << PCoreACCoreFrequency << std::endl
              << "PCore DC Freq " << PCoreDCCoreFrequency << std::endl;
    if (IsHybrid)
    {

        Ret = FrequencyLimiter.GetCoreMaxFrequency(ECORE, ECoreACCoreFrequency, ECoreDCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        // output ecore ac dc frequencies
        std::cout << "ECore AC Freq " << ECoreACCoreFrequency << std::endl
                  << "ECore DC Freq " << ECoreDCCoreFrequency << std::endl;
    }

    std::cout << std::endl
              << "Press enter key to continue..." << std::endl;
    std::cin.get();

    std::cout << std::endl
              << "Test Gear Up " << gearupValue << std::endl;
    FrequencyLimiter.GearUp(gearupValue);

    // output pcore ac dc frequencies
    Ret = FrequencyLimiter.GetCoreMaxFrequency(PCORE, PCoreACCoreFrequency, PCoreDCCoreFrequency);
    if (Ret != 0)
    {
        goto GracefulExit;
    }

    std::cout << "PCore AC Freq " << PCoreACCoreFrequency << std::endl
              << "PCore DC Freq " << PCoreDCCoreFrequency << std::endl;
    if (IsHybrid)
    {

        Ret = FrequencyLimiter.GetCoreMaxFrequency(ECORE, ECoreACCoreFrequency, ECoreDCCoreFrequency);
        if (Ret != 0)
        {
            goto GracefulExit;
        }
        // output ecore ac dc frequencies
        std::cout << "ECore AC Freq " << ECoreACCoreFrequency << std::endl
                  << "ECore DC Freq " << ECoreDCCoreFrequency << std::endl;
    }
    //    std::cout << std::endl << "Press enter key to continue..." << std::endl;
    //    std::cin.get();

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
