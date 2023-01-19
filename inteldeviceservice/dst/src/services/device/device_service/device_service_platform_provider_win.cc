// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/device_service/device_service_platform_provider_win.h"
#include <fstream>
#include <iostream>

namespace device {

DeviceServicePlatformProviderWin::DeviceServicePlatformProviderWin() = default;

DeviceServicePlatformProviderWin::~DeviceServicePlatformProviderWin() = default;

//rrw
std::ofstream o;

mojom::DeviceServiceType DeviceServicePlatformProviderWin::GetDeviceService() {
  o << "test";
  return mojom::DeviceServiceType::kComputeCapacity;
}

void DeviceServicePlatformProviderWin::StartListening() {
  o.open("bcap.txt");

}

void DeviceServicePlatformProviderWin::StopListening() {
  o.close();
}

}  // namespace device
