// frequencylimiter.cc

// headers
#include <windows.h>
#include <string>
#include <powersetting.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <guiddef.h>
#include <rpc.h>
#include <tchar.h>
#include "HybridDetect.h"
#include "frequencyLimiter.h"

// frequencylimiter class

#pragma comment(lib, "pdh.lib")

// frequencyLimiter class constructor
frequencyLimiter::frequencyLimiter()
{

    bool IsOSHybridCapable;
    PROCESSOR_INFO procInfo;

    m_IsHybrid = false;

    // rrw
    m_MaxFrequency = CalculateFrequency();
    uint32_t MinFreq = SetMinFrequency();
    m_CurrentGear = MAX_DOWN_STEPPING;

    // rrw

    GetProcessorInfo(procInfo);
    m_IsHybrid = procInfo.hybrid;
    if (m_IsHybrid)
    {
        m_PCoreGuid = GUID_PROCESSOR_FREQUENCY_LIMIT_1;
        m_ECoreGuid = GUID_PROCESSOR_FREQUENCY_LIMIT;
    }
    else
    {
        m_PCoreGuid = GUID_PROCESSOR_FREQUENCY_LIMIT;
    }

    SetCoreMaxFrequency(PCORE, 0, 0);
    if (m_IsHybrid)
    {
        SetCoreMaxFrequency(ECORE, 0, 0);
    }
}

// frequencyLimiter class destructor
frequencyLimiter::~frequencyLimiter()
{
    SetCoreMaxFrequency(PCORE, 0, 0);
    if (m_IsHybrid)
    {
        SetCoreMaxFrequency(ECORE, 0, 0);
    }
}

// frequencyLimiter::calculateFrequency
uint32_t frequencyLimiter::CalculateFrequency()
{
    std::vector<double> frequency;
    double instance;
    for (auto i = 0; i < SAMPLES_FOR_FREQ; i++)
    {
        unsigned long long tsc1 = __rdtsc();
        Sleep(10);
        unsigned long long tsc2 = __rdtsc();
        instance = (double)(tsc2 - tsc1) / 10000.0;
        frequency.push_back(instance);
    }

    /* geomean */
#ifdef USE_GEOMEAN
    double product = 1.0;
    double gmean;
    for (const auto &number : frequency)
    {
        product *= number;
    }
    gmean = std::pow(product, 1.0 / frequency.size());
    return static_cast<uint32_t>(geomean);
#else
    /* average */
    double sum = std::accumulate(frequency.begin(), frequency.end(), 0);
    double average = (double)sum / frequency.size();

    return static_cast<uint32_t>(average);
#endif
}

// frequencyLimiter::IsHybridCore
int frequencyLimiter::IsHybridCore(bool &IsHybrid)
{
    IsHybrid = m_IsHybrid;
    return 0;
}

// frequencyLimiter::SetCoreMaxFrequency
int frequencyLimiter::SetCoreMaxFrequency(int Core, uint32_t ACMaxFrequency, uint32_t DCMaxFrequency)
{
    DWORD ReturnValue;
    GUID *guid;

    guid = new GUID();
    if (guid == NULL)
    {
        ReturnValue = GUID_ALLOCATION_ERROR;
        goto GracefulExit;
    }

    ReturnValue = PowerGetActiveScheme(NULL, &guid);
    if (ReturnValue != ERROR_SUCCESS)
    {
        goto GracefulExit;
    }
    if (Core == PCORE)
    {
        ReturnValue = PowerWriteACValueIndex(NULL, guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &m_PCoreGuid, ACMaxFrequency);
        if (ReturnValue != ERROR_SUCCESS)
        {
            goto GracefulExit;
        }
        ReturnValue = PowerWriteDCValueIndex(NULL, guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &m_PCoreGuid, DCMaxFrequency);
        if (ReturnValue != ERROR_SUCCESS)
        {
            goto GracefulExit;
        }
    }
    else
    {
        if (m_IsHybrid == false)
        {
            goto GracefulExit;
        }
        ReturnValue = PowerWriteACValueIndex(NULL, guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &m_ECoreGuid, ACMaxFrequency);
        if (ReturnValue != ERROR_SUCCESS)
        {
            goto GracefulExit;
        }
        ReturnValue = PowerWriteDCValueIndex(NULL, guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &m_ECoreGuid, DCMaxFrequency);
        if (ReturnValue != ERROR_SUCCESS)
        {
            goto GracefulExit;
        }
    }
    ReturnValue = PowerSetActiveScheme(NULL, guid);

GracefulExit:
    return ReturnValue;
}

// frequencyLimiter::GetCoreMaxFrequency
int frequencyLimiter::GetCoreMaxFrequency(int Core, uint32_t &ACMaxFrequency, uint32_t &DCMaxFrequency)
{
    DWORD ReturnValue;
    HKEY hKey;
    GUID *guid;
    DWORD dwValue = 0;
    DWORD dwSize = sizeof(dwValue);
    wchar_t RegKey0[2048] = L"";
    wchar_t *guid_string = NULL;
    wchar_t *subgroup_guid_string = NULL;
    wchar_t *freq_limit_guid_string = NULL;
    RPC_STATUS rpcStatus;

    if ((m_IsHybrid == false) && (Core == ECORE))
    {
        ReturnValue = NON_HYBRID_CORE_ECORE_REQUEST;
        goto GracefulExit;
    }

    guid = new GUID();
    if (guid == NULL)
    {
        ReturnValue = GUID_ALLOCATION_ERROR;
        goto GracefulExit;
    }

    wcscat_s(RegKey0, L"SYSTEM\\CurrentControlSet\\Control\\Power\\User\\PowerSchemes\\");
    ReturnValue = PowerGetActiveScheme(NULL, &guid);
    if (ReturnValue != ERROR_SUCCESS)
    {
        goto GracefulExit;
    }
    UuidToString(guid, (RPC_WSTR *)&guid_string);
    wcscat_s(RegKey0, guid_string);
    wcscat_s(RegKey0, L"\\");
    rpcStatus = RpcStringFree((RPC_WSTR *)&guid_string);
    UuidToString(&GUID_PROCESSOR_SETTINGS_SUBGROUP, (RPC_WSTR *)&subgroup_guid_string);
    wcscat_s(RegKey0, subgroup_guid_string);
    rpcStatus = RpcStringFree((RPC_WSTR *)&subgroup_guid_string);
    if (Core == PCORE)
    {
        rpcStatus = UuidToString(&m_PCoreGuid, (RPC_WSTR *)&freq_limit_guid_string);
        if (rpcStatus != RPC_S_OK)
        {
            ReturnValue = rpcStatus;
            goto GracefulExit;
        }
        wcscat_s(RegKey0, L"\\");
        wcscat_s(RegKey0, freq_limit_guid_string);
    }
    else
    {
        rpcStatus = UuidToString(&m_ECoreGuid, (RPC_WSTR *)&freq_limit_guid_string);
        if (rpcStatus != RPC_S_OK)
        {
            ReturnValue = rpcStatus;
            goto GracefulExit;
        }
        wcscat_s(RegKey0, L"\\");
        wcscat_s(RegKey0, freq_limit_guid_string);
    }

    ReturnValue = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPWSTR)RegKey0, 0, KEY_READ, &hKey);
    if (ReturnValue != ERROR_SUCCESS)
    {
        goto GracefulExit;
    }

    // Read the value data.
    ReturnValue = RegQueryValueEx(hKey, L"ACSettingIndex", 0, NULL, (LPBYTE)&dwValue, &dwSize);
    if (ReturnValue != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        goto GracefulExit;
    }
    ACMaxFrequency = dwValue;
    ReturnValue = RegQueryValueEx(hKey, L"DCSettingIndex", 0, NULL, (LPBYTE)&dwValue, &dwSize);
    if (ReturnValue != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        goto GracefulExit;
    }
    DCMaxFrequency = dwValue;

    // Close the registry key.
    RegCloseKey(hKey);

GracefulExit:
    return ReturnValue;
}

// TODO: SetMinFrequency: this will be use to set the bottom limit frequence to x% of max frequency
uint32_t frequencyLimiter::SetMinFrequency()
{
    m_MinFrequency = m_MaxFrequency / MAX_DOWN_STEPPING;
    return m_MinFrequency;
}

// TODO: SetDownStepping: this will be used to set the step size max freq % min freq size
uint32_t frequencyLimiter::SetDownStepping(uint32_t Stepping)
{

    if (Stepping < MAX_DOWN_STEPPING)
    {
        m_DownStepping = Stepping;
    }
    else
    {
        m_DownStepping = MAX_DOWN_STEPPING;
    }
    m_DownGearFrequency = m_MaxFrequency / m_DownStepping;
    return m_DownStepping;
}

// TODO: SetUpStepping: this will be used to set the step size max freq % min freq size
uint32_t frequencyLimiter::SetUpStepping(uint32_t Stepping)
{
    if (Stepping > MIN_UP_STEPPING)
    {
        m_DownStepping = Stepping;
    }
    else
    {
        m_UpStepping = MIN_UP_STEPPING;
    }
    m_UpGearFrequency = m_MaxFrequency / m_UpStepping;
    return m_UpStepping;
}

int frequencyLimiter::GearDown(int32_t Count)
{
    if (m_CurrentGear - Count > 0)
    {
        float CurrentGear, Max_Gear_Stepping;

        m_CurrentGear -= Count;
        CurrentGear = (float ) m_CurrentGear;
        Max_Gear_Stepping = (float ) MAX_DOWN_STEPPING;
        m_CurrentACFrequency = (CurrentGear / Max_Gear_Stepping) * m_MaxFrequency;
        m_CurrentDCFrequency = m_CurrentACFrequency - 200;
        SetCoreMaxFrequency(PCORE, m_CurrentACFrequency, m_CurrentDCFrequency);
        if (m_IsHybrid)
        {
            SetCoreMaxFrequency(ECORE, m_CurrentACFrequency - ECORE_OFFSET, m_CurrentDCFrequency - ECORE_OFFSET);
        }
        else
        {
            m_CurrentGear = 0;
        }
    }
    else
    {
        m_CurrentGear = 0;
        m_CurrentACFrequency = m_MinFrequency;
        m_CurrentDCFrequency = m_MinFrequency - 200;
        SetCoreMaxFrequency(PCORE, m_CurrentACFrequency, m_CurrentDCFrequency);
        if (m_IsHybrid)
        {
            SetCoreMaxFrequency(ECORE, m_CurrentACFrequency - ECORE_OFFSET, m_CurrentDCFrequency - ECORE_OFFSET);
        }
    }

    return 0;
}

int frequencyLimiter::GearUp(int32_t Count)
{
    float CurrentGear, Max_Gear_Stepping;

    if (m_CurrentGear + Count < MAX_DOWN_STEPPING)
    {
        m_CurrentGear += Count;
        CurrentGear = (float)m_CurrentGear;
        Max_Gear_Stepping = (float)MAX_DOWN_STEPPING;
        m_CurrentACFrequency = (CurrentGear / Max_Gear_Stepping) * m_MaxFrequency;
        m_CurrentDCFrequency = m_CurrentACFrequency - 200;
        SetCoreMaxFrequency(PCORE, m_CurrentACFrequency, m_CurrentDCFrequency);
        if (m_IsHybrid)
        {
            SetCoreMaxFrequency(ECORE, m_CurrentACFrequency - ECORE_OFFSET, m_CurrentDCFrequency - ECORE_OFFSET);
        }
    }
    else
    {
        m_CurrentGear = MAX_DOWN_STEPPING;
        SetCoreMaxFrequency(PCORE, 0, 0);
        if (m_IsHybrid)
        {
            SetCoreMaxFrequency(ECORE, 0, 0);
        }
        m_CurrentACFrequency = m_MaxFrequency;
        m_CurrentDCFrequency = m_MaxFrequency - 200;
    }
    return 0;
}