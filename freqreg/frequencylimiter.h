#pragma once

// core types for this class (p and e)
#define PCORE 0
#define ECORE 1

// error codes
#define NON_HYBRID_CORE_ECORE_REQUEST -2
#define GUID_ALLOCATION_ERROR -3

// frequencyLimiter class
class frequencyLimiter
{
private:
	bool m_IsHybrid;

public:
	frequencyLimiter();
	~frequencyLimiter();
	// IsHybridCore: returns if core is OS and hybrid
	int IsHybridCore(bool &IsHybrid);
	// SetCoreMaxFrequency: Sets specified core max frequencies for AC and DC
	int SetCoreMaxFrequency(int CoreType, int ACMaxFrequency, int DCMaxFrequency);
	// GetCoreMaxFrequency: Gets specified core max frequencies for AC and DC
	int GetCoreMaxFrequency(int CoreType, int &ACMaxFrequency, int &DCMaxFrequency);
	// SetMinFrequency: this will be use to set the bottom limit frequence to x% of max frequency
	int SetMinFrequency(int MinFrequency);
	// SetDownStepping: this will be used to set the step size max freq % min freq size
	int SetDownStepping(int Stepping);
	// SetUpStepping: this will be used to set the step size max freq % min freq size
	int SetUpStepping(int Stepping);
};
