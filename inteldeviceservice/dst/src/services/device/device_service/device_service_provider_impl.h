// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PROVIDER_IMPL_H_
#define SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PROVIDER_IMPL_H_

#include <map>
#include <memory>
#include <string>

#include "base/containers/flat_map.h"
#include "base/containers/flat_set.h"
#include "base/task/single_thread_task_runner.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote_set.h"
#include "services/device/public/mojom/device_service_provider.mojom.h"
#include "chrome/browser/buildflags.h"

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
  using ProcessThreadId = std::pair<uint32_t, uint32_t>;

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
  void StartCollectingLbr(uint32_t process_id, uint32_t thread_id) override;
  void StopCollectingLbr(uint32_t process_id, uint32_t thread_id) override;
  void GetLbrData(uint32_t process_id,
                  uint32_t thread_id,
                  GetLbrDataCallback callback) override;

  void OnReceiverConnectionError();

  void OnRecieveLbrData();

  // Since we are currently only performing updates at the system level, we
  // will collapse all requesting into a single set of variables.
  uint32_t last_update_;
  mojom::Capacity last_capacity_;

  std::unique_ptr<DeviceServicePlatformProvider> platform_provider_;
  mojo::ReceiverSet<mojom::DeviceServiceProvider> receivers_;
  mojo::RemoteSet<mojom::DeviceServiceProviderClient> clients_;

  base::flat_map<mojo::ReceiverId, base::flat_set<ProcessThreadId>>
      receiver_requests_;
  base::flat_map<ProcessThreadId, mojom::LbrData> lbr_data_;
  //TODO: we should probably merge these 2 maps
  base::flat_map<ProcessThreadId, uint32_t> request_counter_;

  base::WeakPtrFactory<DeviceServiceProviderImpl> weak_ptr_factory_{this};
};

}  // namespace device

#endif  // SERVICES_DEVICE_DEVICE_SERVICE_DEVICE_SERVICE_PROVIDER_IMPL_H_
