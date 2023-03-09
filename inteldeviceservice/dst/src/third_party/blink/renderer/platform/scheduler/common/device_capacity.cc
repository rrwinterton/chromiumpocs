#include "device_capacity.h"

#include <string.h>
#include <windows.h>
#include <iostream>
#include "base/logging.h"
#include "base/threading/sequence_local_storage_slot.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/device/public/mojom/device_service_provider.mojom-blink.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/platform.h"

constexpr float kWeightDecay = 0.15;

// constructor
DeviceCapacity::DeviceCapacity() noexcept {}

// destructor
DeviceCapacity::~DeviceCapacity() noexcept {}

device::mojom::blink::DeviceServiceProvider& DeviceCapacity::Provider() {
  static base::SequenceLocalStorageSlot<
      mojo::Remote<device::mojom::blink::DeviceServiceProvider>>
      remote_slot;

  mojo::Remote<device::mojom::blink::DeviceServiceProvider>& remote =
      remote_slot.GetOrCreateValue();

  if (!remote) {
    blink::Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
        remote.BindNewPipeAndPassReceiver());
  }
  return *remote.get();
}

// setCapacity
void DeviceCapacity::SetCapacity(float current_capacity) {

  uint32_t tick_count = GetTickCount();
  // TODO: Switch to PlatformThread::CurrentId()
  DWORD id = ::GetCurrentThreadId();
  if ((tick_count != previous_tick_count_) || (id != previous_thread_id_)) {
    previous_tick_count_ = tick_count;
    previous_thread_id_ = id;

    if (current_capacity > high_capacity_) {
      high_capacity_ = current_capacity;
    }
    weighted_capacity_ = (current_capacity * kWeightDecay) +
                         (weighted_capacity_ * (1 - kWeightDecay));
    tick_count = GetTickCount();

    device::mojom::Capacity capacity;

    if (weighted_capacity_ > 0.30) {
      capacity = device::mojom::Capacity::kCapacityOver;
    } else if ((weighted_capacity_ <= 0.30) && (weighted_capacity_ > 0.15)) {
      capacity = device::mojom::Capacity::kCapacityMeet;
    } else if ((weighted_capacity_ <= 0.15) && (weighted_capacity_ > 0.10)) {
      capacity = device::mojom::Capacity::kCapacityUnder;
    } else {
      if ((idle_count_ % 64) == 0) {
        capacity = device::mojom::Capacity::kCapacityIdle;
        idle_count_ = 1;
      } else {
        idle_count_++;
        return;
      }
    }

    Provider().SubmitTaskCapacityHint(tick_count, ::GetCurrentProcessId(), id,
                                      capacity,
                                      base::BindOnce([](int32_t request_id) {
                                        LOG(ERROR) << "RequestID " << request_id;
                                      }));
  }
}

// getweighted_capacity_
void DeviceCapacity::GetWeightedCapacity(float& Capacity) {
  Capacity = weighted_capacity_;
}

// gethigh_capacity_
void DeviceCapacity::GetHighCapacity(float& Capacity) {
  Capacity = high_capacity_;
}
