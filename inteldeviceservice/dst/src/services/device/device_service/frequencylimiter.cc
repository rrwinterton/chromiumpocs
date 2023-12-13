// frequencylimiter.cc

// headers
#include "frequencyLimiter.h"
#include <guiddef.h>
#include <intrin.h>
#include <powersetting.h>
#include <rpc.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

// frequencylimiter class

// frequencyLimiter class constructor
frequencyLimiter::frequencyLimiter()
{
  m_IsHybrid = false;

  // rrw
  m_CurrentGear = MAX_KNOBS;
  if(!m_bCalculatedMaxFreq)
  {
    CalculateFrequency();
    SetMinFrequency(); // TODO:rrw
  }
  m_bCalculatedMaxFreq = true;

  m_IsHybrid = IsIntelHybrid();
  if (m_IsHybrid == true)
  {
    m_PCoreGuid = GUID_PROCESSOR_FREQUENCY_LIMIT_1;
    m_ECoreGuid = GUID_PROCESSOR_FREQUENCY_LIMIT;
  }
  else
  {
    m_PCoreGuid = GUID_PROCESSOR_FREQUENCY_LIMIT;
  }
  activeScheme = new GUID();
  PowerGetActiveScheme(NULL, &activeScheme);

  SetCoreMaxFrequency(PCORE, 0, 0);
  if (m_IsHybrid == true)
  {
    SetCoreMaxFrequency(ECORE, 0, 0);
  }

  IPF_etw_register();

}

// frequencyLimiter class destructor
frequencyLimiter::~frequencyLimiter()
{
  SetCoreMaxFrequency(PCORE, 0, 0);
  if (m_IsHybrid == true)
  {
    SetCoreMaxFrequency(ECORE, 0, 0);
  }

  IPF_etw_unregister();
}

// frequencyLimiter::calculateFrequency
int32_t frequencyLimiter::CalculateFrequency()
{
  std::vector<double> frequencies;
  LARGE_INTEGER freq;
  LARGE_INTEGER start, end;
  DWORD64 tsc1, tsc2, tscDelta;
  double elapsed, frequency;
#ifdef USE_GEOMEAN
  double product;
  double gmean;
#else
  double sum;
  double average;
#endif

  if (m_MaxFrequency == 0)
  {
    if (!QueryPerformanceFrequency(&freq))
    {
      return CALCULATE_FREQUENCY_ERROR;
    }
    for (int i = 0; i < SAMPLES_FOR_FREQ; i++)
    {
      QueryPerformanceCounter(&start);
      tsc1 = __rdtsc();

      // do something
#ifdef USE_GEOMEAN
      product = 1.0;
      for (const auto &number : frequencies)
      {
        product *= number;
      }
      gmean = std::pow(product, 1.0 / frequencies.size());
      m_MaxFrequency = static_cast<uint32_t>(gmean);
#else
      sum = std::accumulate(frequencies.begin(), frequencies.end(), 0);
      average = (double)sum / frequencies.size();
#endif
      Sleep(1);
      tsc2 = __rdtsc();
      QueryPerformanceCounter(&end);
      elapsed = (end.QuadPart - start.QuadPart) / (double)freq.QuadPart;
      tscDelta = tsc2 - tsc1;
      frequency =
          ((tscDelta / elapsed) / 1000000) + ADL_TURBO; // TODO:rrw ADL_TURBO?
      frequencies.push_back(frequency);
    }

    /* geomean */
#ifdef USE_GEOMEAN
    product = 1.0;
    for (const auto &number : frequencies)
    {
      product *= number;
    }
    gmean = std::pow(product, 1.0 / frequencies.size());
    m_MaxFrequency = static_cast<uint32_t>(gmean);
#else
    /* average */
    sum = std::accumulate(frequencies.begin(), frequencies.end(), 0);
    average = (double)sum / frequencies.size();
    m_MaxFrequency = static_cast<uint32_t>(average);
#endif
  }
  return 0;
}

bool frequencyLimiter::IsIntelHybrid()
{
  int cpuInfo[4];
  __cpuid(cpuInfo, 0x7);
  m_IsHybrid = cpuInfo[3] & 0x8000;
  return m_IsHybrid;
}

// frequencyLimiter::IsHybridCore
int32_t frequencyLimiter::IsHybridCore(bool &IsHybrid)
{
  IsHybrid = m_IsHybrid;
  return 0;
}

// frequencyLimiter::SetCoreMaxFrequency
int32_t frequencyLimiter::SetCoreMaxFrequency(int Core,
                                              uint32_t ACMaxFrequency,
                                              uint32_t DCMaxFrequency)
{
  DWORD ReturnValue = ERROR_SUCCESS;
  
  if (Core == PCORE)
  {
    ReturnValue =
        PowerWriteACValueIndex(NULL, activeScheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP,
                               &m_PCoreGuid, ACMaxFrequency);
    if (ReturnValue != ERROR_SUCCESS)
    {
      goto GracefulExit;
    }
    ReturnValue =
        PowerWriteDCValueIndex(NULL, activeScheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP,
                               &m_PCoreGuid, DCMaxFrequency);
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
    ReturnValue =
        PowerWriteACValueIndex(NULL, activeScheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP,
                               &m_ECoreGuid, ACMaxFrequency);
    if (ReturnValue != ERROR_SUCCESS)
    {
      goto GracefulExit;
    }
    ReturnValue =
        PowerWriteDCValueIndex(NULL, activeScheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP,
                               &m_ECoreGuid, DCMaxFrequency);
    if (ReturnValue != ERROR_SUCCESS)
    {
      goto GracefulExit;
    }
  }
  ReturnValue = PowerSetActiveScheme(NULL, activeScheme);

GracefulExit:
  return ReturnValue;
}

// frequencyLimiter::GetCoreMaxFrequency
int32_t frequencyLimiter::GetCoreMaxFrequency(int Core,
                                              uint32_t &ACMaxFrequency,
                                              uint32_t &DCMaxFrequency)
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

  wcscat_s(RegKey0,
           L"SYSTEM\\CurrentControlSet\\Control\\Power\\User\\PowerSchemes\\");
  ReturnValue = PowerGetActiveScheme(NULL, &guid);
  if (ReturnValue != ERROR_SUCCESS)
  {
    goto GracefulExit;
  }
  UuidToString(guid, (RPC_WSTR *)&guid_string);
  wcscat_s(RegKey0, guid_string);
  wcscat_s(RegKey0, L"\\");
  rpcStatus = RpcStringFree((RPC_WSTR *)&guid_string);
  UuidToString(&GUID_PROCESSOR_SETTINGS_SUBGROUP,
               (RPC_WSTR *)&subgroup_guid_string);
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

  ReturnValue =
      RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPWSTR)RegKey0, 0, KEY_READ, &hKey);
  if (ReturnValue != ERROR_SUCCESS)
  {
    goto GracefulExit;
  }

  // Read the value data.
  ReturnValue = RegQueryValueEx(hKey, L"ACSettingIndex", 0, NULL,
                                (LPBYTE)&dwValue, &dwSize);
  if (ReturnValue != ERROR_SUCCESS)
  {
    RegCloseKey(hKey);
    goto GracefulExit;
  }
  ACMaxFrequency = dwValue;
  ReturnValue = RegQueryValueEx(hKey, L"DCSettingIndex", 0, NULL,
                                (LPBYTE)&dwValue, &dwSize);
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

// TODO: SetMinFrequency: this will be use to set the bottom limit frequence to
// x% of max frequency
uint32_t frequencyLimiter::SetMinFrequency()
{
  m_MinFrequency = m_MaxFrequency / MIN_FREQUENCY_SCALE;
  return m_MinFrequency;
}

// TODO: SetDownStepping: this will be used to set the step size max freq % min
// freq size
/*
uint32_t frequencyLimiter::SetSteppingKnobs(uint32_t Stepping)
{

    if (Stepping < MAX_KNOBS)
    {
        m_SteppingScale = Stepping;
    }
    else
    {
        m_SteppingScale = MAX_KNOBS;
    }
    return 0;
}
*/

int32_t frequencyLimiter::GearDown(int32_t Count)
{
  float CurrentGear, Max_Gear_Stepping;
  uint32_t dcOffset, eCoreOffset;

  if (((0 == Count) || (Count > MAX_KNOBS)) || ((m_CurrentACFrequency <= 1000) && (m_CurrentACFrequency != 0)))
  {
    goto GracefulExit;
  }
  IPF_etw_log_GearDown(0, 0);
  if (m_CurrentACFrequency == 0) 
  {
    m_CurrentACFrequency = m_MaxFrequency;
  }

  if (m_CurrentGear - Count > 0)
  {
    m_CurrentGear -= Count;
    CurrentGear = (float)m_CurrentGear;
    Max_Gear_Stepping = (float)MAX_KNOBS;
    m_CurrentACFrequency = (CurrentGear / Max_Gear_Stepping) * m_MaxFrequency;
    if ((m_CurrentACFrequency == 0) || (m_CurrentACFrequency > 1000))
    {
      dcOffset = 200; // rrw fix magic numbers
      eCoreOffset = ECORE_OFFSET;
    }
    else
    {
      m_CurrentACFrequency = 1000;
      dcOffset = 0;
      eCoreOffset = 0;
    }
    m_CurrentDCFrequency = m_CurrentACFrequency - dcOffset;
    SetCoreMaxFrequency(PCORE, m_CurrentACFrequency, m_CurrentDCFrequency);
    if (m_IsHybrid == true)
    {
      SetCoreMaxFrequency(ECORE, m_CurrentACFrequency - eCoreOffset,
                          m_CurrentDCFrequency - eCoreOffset);
    }
  }
  else
  {
    m_CurrentGear = 0;
    if (m_CurrentACFrequency > 1000)
    {
      dcOffset = 200; // rrw fix magic numbers
      eCoreOffset = ECORE_OFFSET;
    }
    else
    {
      dcOffset = 0;
      eCoreOffset = 0;
    }
    m_CurrentDCFrequency = m_CurrentACFrequency - dcOffset;    
    SetCoreMaxFrequency(PCORE, m_CurrentACFrequency, m_CurrentDCFrequency);
    if (m_IsHybrid == true)
    {
      SetCoreMaxFrequency(ECORE, m_CurrentACFrequency - eCoreOffset,
                          m_CurrentDCFrequency - eCoreOffset);
    }
  }
GracefulExit:
  return 0;
}

int32_t frequencyLimiter::GearUp(int32_t Count)
{
  float CurrentGear, Max_Gear_Stepping;
  uint32_t dcOffset, eCoreOffset;
  if ((m_CurrentACFrequency == 0) || (0 == Count) || (Count > MAX_KNOBS))
  {
    goto GracefulExit;
  }
  IPF_etw_log_GearUp(0, 0);
  if (m_CurrentGear + Count < MAX_KNOBS)
  {
    m_CurrentGear += Count;
    CurrentGear = (float) m_CurrentGear;
    Max_Gear_Stepping = (float) MAX_KNOBS;
    m_CurrentACFrequency = (CurrentGear / Max_Gear_Stepping) * m_MaxFrequency;
    if (m_CurrentACFrequency >= 1000)
    {
      dcOffset = 200; // rrw fix magic numbers
      eCoreOffset = ECORE_OFFSET;
      m_CurrentDCFrequency = m_CurrentACFrequency - dcOffset;
    }
    else
    {
      dcOffset = 0;
      eCoreOffset = 0;
    }
    SetCoreMaxFrequency(PCORE, m_CurrentACFrequency, m_CurrentDCFrequency);
    if (m_IsHybrid == true)
    {
      SetCoreMaxFrequency(ECORE, m_CurrentACFrequency - eCoreOffset,
                          m_CurrentDCFrequency - eCoreOffset);
    }
  }
  else
  {
    m_CurrentGear = MAX_KNOBS;
    SetCoreMaxFrequency(PCORE, 0, 0);
    if (m_IsHybrid == true)
    {
      SetCoreMaxFrequency(ECORE, 0, 0);
    }
    m_CurrentACFrequency = 0;
    m_CurrentDCFrequency = 0;
  }
GracefulExit:
  return 0;
}
