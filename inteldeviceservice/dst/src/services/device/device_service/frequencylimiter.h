#pragma once

#include <windows.h>
#include <string>
#include <guiddef.h>
#include <rpc.h>

// core types for this class (p and e)
#define PCORE 0			 // for registry key and scaling
#define ECORE 1			 // for registry key and scaling
#define ECORE_OFFSET 100 // less frequency for ecore
#define MAX_KNOBS 100	 // number of knobs
#define MIN_FREQUENCY_SCALE 10
#define MIN_UP_STEPPING 1	//
#define SAMPLES_FOR_FREQ 15 // samples for max frequency
//#define USE_GEOMEAN 1
#define DO_UNIT_TEST 1
#define ADL_TURBO 1500

// error codes
#define ERROR_INPUT -1
#define NON_HYBRID_CORE_ECORE_REQUEST -2
#define GUID_ALLOCATION_ERROR -3
#define CALCULATE_FREQUENCY_ERROR -4

static bool m_bCalculatedMaxFreq = false;
static uint32_t m_MaxFrequency = 0;
static uint32_t m_MinFrequency = 0;

// frequencyLimiter class
class frequencyLimiter
{
private:
	// methods
	uint32_t SetMinFrequency();
	int SetCoreMaxFrequency(int CoreType, uint32_t ACMaxFrequency, uint32_t DCMaxFrequency); // sets core max frequencies for AC and DC
	bool IsIntelHybrid();
	// variables
	uint32_t m_CurrentACFrequency;
	uint32_t m_CurrentDCFrequency;
	int32_t m_CurrentGear;
	bool m_IsHybrid;
	GUID m_PCoreGuid;
	GUID m_ECoreGuid;
	GUID *activeScheme;
	//	float m_SteppingScale;
#ifndef DO_UNIT_TEST
	// GetCoreMaxFrequency: Gets specified core max frequencies for AC and DC
	int GetCoreMaxFrequency(int CoreType, uint32_t &ACMaxFrequency, uint32_t &DCMaxFrequency);
#endif

public:
	frequencyLimiter();
	~frequencyLimiter();
	int32_t CalculateFrequency();
	int IsHybridCore(bool &IsHybrid); // Returns true if core is hybrid doesn't check OS yet
	int32_t GearDown(int32_t Count);
	int32_t GearUp(int32_t Count);

#ifdef DO_UNIT_TEST
	int GetCoreMaxFrequency(int CoreType, uint32_t &ACMaxFrequency, uint32_t &DCMaxFrequency); // Specified core max frequencies for AC and DC
#endif
};