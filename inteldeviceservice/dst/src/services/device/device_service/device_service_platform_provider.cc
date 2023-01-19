// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/device_service/device_service_platform_provider.h"

#include "build/build_config.h"
#include "services/device/device_service/device_service_provider_impl.h"

#if BUILDFLAG(IS_WIN)
#include "services/device/device_service/device_service_platform_provider_win.h"
#endif

namespace device {

// static
std::unique_ptr<DeviceServicePlatformProvider>
DeviceServicePlatformProvider::Create() {
#if BUILDFLAG(IS_WIN)
  return std::make_unique<DeviceServicePlatformProviderWin>();
#else
  return nullptr;
#endif
}

void DeviceServicePlatformProvider::SetServiceProvider(
    DeviceServiceProviderImpl* provider) {
  provider_ = provider;
}

void DeviceServicePlatformProvider::NotifyDeviceServiceChanged(
    const device::mojom::DeviceServiceType& service) {
  DCHECK(provider_);
  provider_->OnDeviceServiceChanged(service);
}

}  // namespace device
