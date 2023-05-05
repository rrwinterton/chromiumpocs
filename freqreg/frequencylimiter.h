#pragma once

#include <windows.h>
#include <string>
#include <guiddef.h>
#include <rpc.h>

// core types for this class (p and e)
#define PCORE 0 //for registry key and scaling
#define ECORE 1 //for registry key and scaling
#define ECORE_OFFSET 100 //less frequency for ecore
#define MAX_KNOBS 100 //number of knobs
#define MIN_FREQUENCY_SCALE 10
#define MIN_UP_STEPPING 1 //
#define SAMPLES_FOR_FREQ 15 //samples for max frequency
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

	//methods
	uint32_t SetMinFrequency();
	uint32_t CalculateFrequency();
	int SetCoreMaxFrequency(int CoreType, uint32_t ACMaxFrequency, uint32_t DCMaxFrequency); 	// sets core max frequencies for AC and DC

	//variables
	uint32_t m_CurrentACFrequency;
	uint32_t m_CurrentDCFrequency;
	uint32_t m_MaxFrequency;
	uint32_t m_MinFrequency;
	uint32_t m_UpGearFrequency;
	int32_t m_CurrentGear;
	bool m_IsHybrid;
	GUID m_PCoreGuid;
	GUID m_ECoreGuid;
	//	float m_SteppingScale;
#ifndef DO_UNIT_TEST
	// GetCoreMaxFrequency: Gets specified core max frequencies for AC and DC
	int GetCoreMaxFrequency(int CoreType, uint32_t& ACMaxFrequency, uint32_t& DCMaxFrequency);
#endif

public:
	frequencyLimiter();
	~frequencyLimiter();
	int IsHybridCore(bool& IsHybrid); // Returns true if core is hybrid doesn't check OS yet
//	uint32_t SetSteppingKnobs(uint32_t Stepping); // Used to set the step size max freq % min freq size
	int GearDown(uint32_t Count);
	int GearUp(uint32_t Count);

#ifdef DO_UNIT_TEST
	int GetCoreMaxFrequency(int CoreType, uint32_t& ACMaxFrequency, uint32_t& DCMaxFrequency); //Specified core max frequencies for AC and DC
#endif
};
