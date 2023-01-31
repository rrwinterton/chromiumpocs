#include "device_capacity.h"
#include <string.h>
#include <windows.h>
#include <iostream>
#include "base/logging.h"
#include "services/device/public/mojom/device_service.mojom.h"

// constructor
Device_Capacity::Device_Capacity() noexcept {}

// destructor
Device_Capacity::~Device_Capacity() noexcept {}

// setCapacity
void Device_Capacity::setCapacity(int id, float currentCapacity) {
  static int sampleCnt = 0;
  int cnt;
  char tmpStr[512];
  char dbgStr[512];
  std::map<int, float>::iterator it;

  if (highCapacity.find(id) == highCapacity.end()) {
    highCapacity[id] = 0.0;
  }
  if (weightedCapacity.find(id) == weightedCapacity.end()) {
    weightedCapacity[id] = 0.0;
  }
  if (currentCapacity > highCapacity[id]) {
    highCapacity[id] = currentCapacity;
  }
  weightedCapacity[id] = (currentCapacity * WEIGHT_DECAY) +
                         (weightedCapacity[id] * (1 - WEIGHT_DECAY));

  strcpy(dbgStr, "tid: ");
  itoa(id, tmpStr, 10);
  strcat(dbgStr, tmpStr);
  if (weightedCapacity[id] > 0.10) {
    sampleCnt = 0;
    if (weightedCapacity[id] > 0.30) {
        strcat(dbgStr, " PERF: weighted capacity: ");
    } else if ((weightedCapacity[id] <= 0.30) && (weightedCapacity[id] > 0.15)) {
        strcat(dbgStr, " NOMINAL: weighted capacity: ");
    } else {
        strcat(dbgStr, " ECO: weighted capacity: ");
    }
    gcvt((float)weightedCapacity[id], 4, tmpStr);
    strcat(dbgStr, tmpStr);
    LOG(ERROR) << dbgStr;
  } else {
    if ((sampleCnt % 1024) == 0) {
  strcpy(dbgStr, "total pairs: ");
      cnt = (int ) weightedCapacity.size();
      itoa(cnt, tmpStr, 10);
      strcat(dbgStr, tmpStr);
      strcat(dbgStr, ", tid: ");
      itoa(id, tmpStr, 10);
      strcat(dbgStr, tmpStr);
      strcat(dbgStr, " Erasing mapped, max value: ");
      gcvt((float)highCapacity[id], 4, tmpStr);
      strcat(dbgStr, tmpStr);
      LOG(ERROR) << dbgStr;
      highCapacity[id];
      it = highCapacity.find(id);
      highCapacity.erase(it);
      it = weightedCapacity.find(id);
      weightedCapacity.erase(it);

    }
    sampleCnt++;
  } 
}

// getWeightedCapacity
void Device_Capacity::getWeightedCapacity(int id, float& Capacity) {
  if (weightedCapacity.find(id) == weightedCapacity.end()) {
    weightedCapacity[id] = 0.0;
  }
  Capacity = weightedCapacity[id];
}

// getHighCapacity
void Device_Capacity::getHighCapacity(int id, float& Capacity) {
  if (highCapacity.find(id) == highCapacity.end()) {
    highCapacity[id] = 0.0;
  }
  Capacity = highCapacity[id];
}
