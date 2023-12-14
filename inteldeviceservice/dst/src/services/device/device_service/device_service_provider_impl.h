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
#include "chrome/browser/buildflags.h"

#include <windows.h>

#ifdef __cplusplus 
#define EXTERN_C extern "C"
#endif

#if BUILDFLAG(ENABLE_IPF)
EXTERN_C int GearUp();    //ipf gear up
EXTERN_C int GearDown();  //ipf gear down
#endif

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
  void SubmitTaskCapacityHint(uint32_t system_time,
                              uint32_t process_id,
                              uint32_t pthread_id,
                              mojom::Capacity capacity,
                              SubmitTaskCapacityHintCallback callback) override;
  void OnReceiverConnectionError();

  // Since we are currently only performing updates at the system level, we will
  // collapse all requesting into a single set of variables.
  uint32_t last_update_;
  uint32_t deviceServiceFlag;
  mojom::Capacity last_capacity_;
  bool isEco = false;

  std::unique_ptr<DeviceServicePlatformProvider> platform_provider_;
  mojo::ReceiverSet<mojom::DeviceServiceProvider> receivers_;
  mojo::RemoteSet<mojom::DeviceServiceProviderClient> clients_;
  base::WeakPtrFactory<DeviceServiceProviderImpl> weak_ptr_factory_{this};
};

}  // namespace device

#endif  // SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PROVIDER_IMPL_H_
