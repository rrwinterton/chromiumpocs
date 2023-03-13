
#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_COMMON_THREAD_DEVICE_CAPACITY_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_COMMON_THREAD_DEVICE_CAPACITY_H_

#include "services/device/public/mojom/device_service_provider.mojom-blink.h"

class DeviceCapacity {
 public:
  DeviceCapacity() noexcept;
  ~DeviceCapacity() noexcept;

  void SetCapacity(float current_capacity);
  void GetWeightedCapacity(float& capacity);
  void GetHighCapacity(float& capacity);

 private:
  device::mojom::blink::DeviceServiceProvider& Provider();

  float high_capacity_;
  float weighted_capacity_;
  uint32_t idle_count_;
  uint32_t previous_tick_count_;
  uint32_t previous_thread_id_;
};

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_COMMON_THREAD_DEVICE_CAPACITY_H_
