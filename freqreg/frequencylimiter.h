#pragma once

#include <windows.h>
#include <string>
#include <guiddef.h>
#include <rpc.h>

// core types for this class (p and e)
#define PCORE 0
#define ECORE 1
#define ECORE_OFFSET 100
#define MAX_DOWN_STEPPING 10
#define MIN_UP_STEPPING 1
#define SAMPLES_FOR_FREQ 15
// #define USE_GEOMEAN 1
#define DO_UNIT_TEST 1

// error codes
#define ERROR_INPUT -1
#define NON_HYBRID_CORE_ECORE_REQUEST -2
#define GUID_ALLOCATION_ERROR -3

// frequencyLimiter class
class frequencyLimiter
{
private:
	uint32_t m_MaxFrequency; // still figuring out best samples and to use geomean or average or other
	uint32_t m_MinFrequency;
	uint32_t m_CurrentACFrequency;
	uint32_t m_CurrentDCFrequency;
	uint32_t m_UpGearFrequency;
	uint32_t m_DownGearFrequency;
	int32_t m_CurrentGear;
	bool m_IsHybrid;
	GUID m_PCoreGuid;
	GUID m_ECoreGuid;
	uint32_t m_DownStepping;
	uint32_t m_UpStepping;
	uint32_t SetMinFrequency();
	uint32_t CalculateFrequency();

	// SetCoreMaxFrequency: Sets specified core max frequencies for AC and DC
	int SetCoreMaxFrequency(int CoreType, uint32_t ACMaxFrequency, uint32_t DCMaxFrequency);
#ifndef DO_UNIT_TEST
	// GetCoreMaxFrequency: Gets specified core max frequencies for AC and DC
	int GetCoreMaxFrequency(int CoreType, uint32_t& ACMaxFrequency, uint32_t& DCMaxFrequency);
#endif

public:
	frequencyLimiter();
	~frequencyLimiter();
	// IsHybridCore: returns if core is OS and hybrid
	int IsHybridCore(bool &IsHybrid);
#ifdef DO_UNIT_TEST
	// GetCoreMaxFrequency: Gets specified core max frequencies for AC and DC
	int GetCoreMaxFrequency(int CoreType, uint32_t& ACMaxFrequency, uint32_t& DCMaxFrequency);
#endif
	// SetDownStepping: this will be used to set the step size max freq % min freq size
	uint32_t SetDownStepping(uint32_t Stepping);
	// SetUpStepping: this will be used to set the step size max freq % min freq size
	uint32_t SetUpStepping(uint32_t Stepping);
	int GearDown(int32_t Count);
	int GearUp(int32_t Count);
};
