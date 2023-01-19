// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/public/cpp/test/fake_device_service_provider.h"

#include <memory>
#include <utility>

namespace device {

FakeDeviceServiceProvider::FakeDeviceServiceProvider() = default;

FakeDeviceServiceProvider::~FakeDeviceServiceProvider() = default;

void FakeDeviceServiceProvider::Bind(
    mojo::PendingReceiver<mojom::DeviceServiceProvider> receiver) {
  receivers_.Add(this, std::move(receiver));
}

void FakeDeviceServiceProvider::AddListenerAndGetCurrentService(
    mojo::PendingRemote<mojom::DeviceServiceProviderClient> client,
    AddListenerAndGetCurrentServiceCallback callback) {
  clients_.Add(std::move(client));
  std::move(callback).Run(current_service_);
}

void FakeDevicePostureProvider::SetCurrentServiceForTesting(
    device::mojom::DeviceServiceType service) {
  current_service_ = service;
  DispatchServiceChanges();
}

void FakeDeviceServiceProvider::DispatchServiceChanges() {
  for (auto& client : clients_) {
    client->OnServiceChanged(current_service_);
  }
}

}  // namespace device
