// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_PUBLIC_CPP_TEST_FAKE_DEVICE_SERVICE_PROVIDER_H_
#define SERVICES_DEVICE_PUBLIC_CPP_TEST_FAKE_DEVICE_SERVICE_PROVIDER_H_

#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "mojo/public/cpp/system/buffer.h"
#include "services/device/public/mojom/device_service_provider.mojom.h"

namespace device {

class FakeDeviceServiceProvider : public mojom::DeviceServiceProvider {
 public:
  FakeDeviceServiceProvider();
  ~FakeDeviceServiceProvider() override;

  FakeDeviceServiceProvider(const FakeDeviceServiceProvider&) = delete;
  FakeDeviceServiceProvider& operator=(const FakeDeviceServiceProvider&) =
      delete;

  // mojom::DeviceServiceProvider:
  void AddListenerAndGetCurrentService(
      mojo::PendingRemote<mojom::DeviceServiceProviderClient> client,
      AddListenerAndGetCurrentServiceCallback callback) override;

  void SetCurrentServiceForTesting(device::mojom::DeviceServiceType service);
  void Bind(mojo::PendingReceiver<mojom::DeviceServiceProvider> receiver);

 private:
  void DispatchServiceChanges();
  mojo::ReceiverSet<mojom::DeviceServiceProvider> receivers_;
  mojo::RemoteSet<mojom::DeviceServiceProviderClient> clients_;
  mojom::DeviceServiceType current_service_ =
      mojom::DeviceServiceType::kComputeCapcity;
};

}  // namespace device

#endif  // SERVICES_DEVICE_PUBLIC_CPP_TEST_FAKE_DEVICE_Service_PROVIDER_H_
