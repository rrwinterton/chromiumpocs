// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PLATFORM_PROVIDER_H_
#define SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PLATFORM_PROVIDER_H_

#include <memory>

#include "base/memory/raw_ptr.h"
#include "services/device/public/mojom/device_service_provider.mojom.h"

namespace device {

class DeviceServiceProviderImpl;

// This the base class for platform-specific device service provider
// implementations. In typical usage a single instance is owned by
// DeviceService.
class DeviceServicePlatformProvider {
 public:
  // Returns a DeviceServiceProvider for the current platform.
  // Note: returns 'nullptr' if there is no available implementation for
  // the current platform.
  static std::unique_ptr<DeviceServicePlatformProvider> Create();

  virtual ~DeviceServicePlatformProvider() = default;

  virtual device::mojom::DeviceServiceType GetDeviceService() = 0;
  virtual void StopListening() = 0;
  virtual void StartListening() = 0;

  void SetServiceProvider(DeviceServiceProviderImpl* provider);

  DeviceServicePlatformProvider(const DeviceServicePlatformProvider&) = delete;
  DeviceServicePlatformProvider& operator=(
      const DeviceServicePlatformProvider&) = delete;

 protected:
  DeviceServicePlatformProvider() = default;
  void NotifyDeviceServiceChanged(const mojom::DeviceServiceType& service);
 private:
  raw_ptr<DeviceServiceProviderImpl> provider_;
};

}  // namespace device

#endif  // SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PLATFORM_PROVIDER_H_
