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
void Device_Capacity::setCapacity(uint32_t id, float currentCapacity) {
  static uint32_t idleCnt = 0;
  static uint32_t prevTicCnt = 0;
  static uint32_t prevTID = 0;
  uint32_t ticCnt;
  char tmpStr[1024];
  char dbgStr[1024];
  std::map<uint32_t, float>::iterator it;

  ticCnt = GetTickCount();
  if ((ticCnt != prevTicCnt) || (id != prevTID)) {
    prevTicCnt = ticCnt;
    prevTID = id;
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
    strcpy(dbgStr, "time, ");
    ticCnt = GetTickCount();
    itoa(ticCnt, tmpStr, 10);
    strcat(dbgStr, tmpStr);
    strcat(dbgStr, ", tid, ");
    itoa(id, tmpStr, 10);
    strcat(dbgStr, tmpStr);
    if (weightedCapacity[id] > 0.30) {
      strcat(dbgStr, ", OVER, ");
      gcvt((float)weightedCapacity[id], 4, tmpStr);
      strcat(dbgStr, tmpStr);
      LOG(ERROR) << dbgStr;
    } else if ((weightedCapacity[id] <= 0.30) &&
               (weightedCapacity[id] > 0.15)) {
      strcat(dbgStr, ", MEET, ");
      gcvt((float)weightedCapacity[id], 4, tmpStr);
      strcat(dbgStr, tmpStr);
      LOG(ERROR) << dbgStr;
    } else if ((weightedCapacity[id] <= 0.15) &&
               (weightedCapacity[id] > 0.10)) {
      strcat(dbgStr, ", UNDER, ");
      gcvt((float)weightedCapacity[id], 4, tmpStr);
      strcat(dbgStr, tmpStr);
      LOG(ERROR) << dbgStr;
    } else {
      if ((idleCnt % 64) == 0) {
        strcat(dbgStr, ", IDLE, ");
        gcvt((float)weightedCapacity[id], 4, tmpStr);
        strcat(dbgStr, tmpStr);
        strcat(dbgStr, ", max was, ");
        gcvt((float) highCapacity[id], 4, tmpStr);
        strcat(dbgStr, tmpStr);
        LOG(ERROR) << dbgStr;
        it = highCapacity.find(id);
        highCapacity.erase(it);
        it = weightedCapacity.find(id);
        weightedCapacity.erase(it);
        idleCnt = 1;
      } else {
        idleCnt++;
      }
    }
  }
}

// getWeightedCapacity
void Device_Capacity::getWeightedCapacity(uint32_t id, float& Capacity) {
  if (weightedCapacity.find(id) == weightedCapacity.end()) {
    weightedCapacity[id] = 0.0;
  }
  Capacity = weightedCapacity[id];
}

// getHighCapacity
void Device_Capacity::getHighCapacity(uint32_t id, float& Capacity) {
  if (highCapacity.find(id) == highCapacity.end()) {
    highCapacity[id] = 0.0;
  }
  Capacity = highCapacity[id];
}
