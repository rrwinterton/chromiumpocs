// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PROVIDER_IMPL_H_
#define SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PROVIDER_IMPL_H_

#include <map>
#include <memory>
#include <string>

#include "base/task/single_thread_task_runner.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "services/device/public/mojom/device_service_provider.mojom.h"

namespace device {

class DeviceServicePlatformProvider;

class DeviceServiceProviderImpl : public mojom::DeviceServiceProvider {
 public:
  explicit DeviceServiceProviderImpl(
      std::unique_ptr<DeviceServicePlatformProvider> service_provider);

  ~DeviceServiceProviderImpl() override;
  DeviceServiceProviderImpl(const DeviceServiceProviderImpl&) = delete;
  DeviceServiceProviderImpl& operator=(const DeviceServiceProviderImpl&) =
      delete;

  // Adds this receiver to |receiverss_|.
  void Bind(mojo::PendingReceiver<mojom::DeviceServiceProvider> receiver);
  void OnDeviceServiceChanged(const mojom::DeviceServiceType& service);

 private:
  // DeviceServiceProvider implementation.
  void AddListenerAndGetCurrentService(
      mojo::PendingRemote<mojom::DeviceServiceProviderClient> client,
      AddListenerAndGetCurrentServiceCallback callback) override;
  void OnReceiverConnectionError();

  std::unique_ptr<DeviceServicePlatformProvider> platform_provider_;
  mojo::ReceiverSet<mojom::DeviceServiceProvider> receivers_;
  mojo::RemoteSet<mojom::DeviceServiceProviderClient> clients_;
  base::WeakPtrFactory<DeviceServiceProviderImpl> weak_ptr_factory_{this};
};

}  // namespace device

#endif  // SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PROVIDER_IMPL_H_
