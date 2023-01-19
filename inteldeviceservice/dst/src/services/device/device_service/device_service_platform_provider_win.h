// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PLATFORM_PROVIDER_WIN_H_
#define SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PLATFORM_PROVIDER_WIN_H_

#include "services/device/device_service/device_service_platform_provider.h"

namespace device {

class DeviceServicePlatformProviderWin : public DeviceServicePlatformProvider {
 public:
  DeviceServicePlatformProviderWin();
  ~DeviceServicePlatformProviderWin() override;

  DeviceServicePlatformProviderWin(const DeviceServicePlatformProviderWin&) =
      delete;
  DeviceServicePlatformProviderWin& operator=(
      const DeviceServicePlatformProviderWin&) = delete;

  device::mojom::DeviceServiceType GetDeviceService() override;
  void StartListening() override;
  void StopListening() override;
};

}  // namespace device

#endif  // SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PLATFORM_PROVIDER_WIN_H_
