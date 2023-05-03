// frequencylimiter.cc

// headers
#include <windows.h>
#include <string>
#include <powersetting.h>
#include <iostream>
#include <guiddef.h>
#include <rpc.h>
#include "HybridDetect.h"
#include "frequencyLimiter.h"

// frequencylimiter class

//frequencyLimiter class constructor
frequencyLimiter::frequencyLimiter()
{

    bool IsOSHybridCapable;
    PROCESSOR_INFO procInfo;

    m_IsHybrid = false;

    IsOSHybridCapable = IsHybridOSEx();
    if (true == IsOSHybridCapable)
    {
        GetProcessorInfo(procInfo);
        m_IsHybrid = procInfo.hybrid;
        // m_MaximumFrequency = procInfo.logicalCore.maximumFrequency;
    }
}

//frequencyLimiter class destructor
frequencyLimiter::~frequencyLimiter() {}

// frequencyLimiter::IsHybridCore
int frequencyLimiter::IsHybridCore(bool &IsHybrid)
{
    IsHybrid = m_IsHybrid;
    return 0;
}

// frequencyLimiter::SetCoreMaxFrequency
int frequencyLimiter::SetCoreMaxFrequency(int Core, int ACMaxFrequency, int DCMaxFrequency)
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
        ReturnValue = PowerWriteACValueIndex(NULL, guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROCESSOR_FREQUENCY_LIMIT, ACMaxFrequency);
        if (ReturnValue != ERROR_SUCCESS)
        {
            goto GracefulExit;
        }
        ReturnValue = PowerWriteDCValueIndex(NULL, guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROCESSOR_FREQUENCY_LIMIT, DCMaxFrequency);
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
        ReturnValue = PowerWriteACValueIndex(NULL, guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROCESSOR_FREQUENCY_LIMIT_1, ACMaxFrequency);
        if (ReturnValue != ERROR_SUCCESS)
        {
            goto GracefulExit;
        }
        ReturnValue = PowerWriteACValueIndex(NULL, guid, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROCESSOR_FREQUENCY_LIMIT_1, DCMaxFrequency);
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
int frequencyLimiter::GetCoreMaxFrequency(int Core, int &ACMaxFrequency, int &DCMaxFrequency)
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
        rpcStatus = UuidToString(&GUID_PROCESSOR_FREQUENCY_LIMIT, (RPC_WSTR *)&freq_limit_guid_string);
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
        rpcStatus = UuidToString(&GUID_PROCESSOR_FREQUENCY_LIMIT_1, (RPC_WSTR *)&freq_limit_guid_string);
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
int SetMinFrequency(int MinFrequency)
{
    return 0;
}

// TODO: SetDownStepping: this will be used to set the step size max freq % min freq size
int SetDownStepping(int Stepping)
{
    return 0;
}

// TODO: SetUpStepping: this will be used to set the step size max freq % min freq size
int SetUpStepping(int Stepping)
{
    return 0;
}
