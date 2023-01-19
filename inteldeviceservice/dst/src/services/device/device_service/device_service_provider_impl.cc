// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/device_service/device_service_provider_impl.h"

#include "base/bind.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "services/device/device_service/device_service_platform_provider.h"

namespace device {

DeviceServiceProviderImpl::DeviceServiceProviderImpl(
    std::unique_ptr<DeviceServicePlatformProvider> platform_provider)
    : platform_provider_(std::move(platform_provider)) {
  DCHECK(platform_provider_);
  platform_provider_->SetServiceProvider(this);
  // We need to  listen to disconnections so that if there is nobody interested
  // in service changes we can shutdown the native backends.
  receivers_.set_disconnect_handler(
      base::BindRepeating(&DeviceServiceProviderImpl::OnReceiverConnectionError,
                          weak_ptr_factory_.GetWeakPtr()));
}

DeviceServiceProviderImpl::~DeviceServiceProviderImpl() = default;

void DeviceServiceProviderImpl::AddListenerAndGetCurrentService(
    mojo::PendingRemote<mojom::DeviceServiceProviderClient> client,
    AddListenerAndGetCurrentServiceCallback callback) {
  clients_.Add(std::move(client));
  mojom::DeviceServiceType service = platform_provider_->GetDeviceService();
  std::move(callback).Run(service);
}

void DeviceServiceProviderImpl::Bind(
    mojo::PendingReceiver<mojom::DeviceServiceProvider> receiver) {
  if (receivers_.empty())
    platform_provider_->StartListening();
  receivers_.Add(this, std::move(receiver));
}

void DeviceServiceProviderImpl::OnDeviceServiceChanged(
    const mojom::DeviceServiceType& service) {
  for (auto& client : clients_)
    client->OnServiceChanged(service);
}

void DeviceServiceProviderImpl::OnReceiverConnectionError() {
  if (receivers_.empty())
    platform_provider_->StopListening();
}

}  // namespace device
