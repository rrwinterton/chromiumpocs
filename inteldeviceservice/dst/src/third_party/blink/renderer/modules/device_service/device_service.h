// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_SERVICE_DEVICE_SERVICE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_SERVICE_DEVICE_SERVICE_H_

#include "services/device/public/mojom/device_service_provider.mojom-blink.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_observer.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_receiver.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_remote.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_wrapper_mode.h"

namespace blink {

class LocalDOMWindow;

class MODULES_EXPORT DeviceService
    : public EventTargetWithInlineData,
      public ExecutionContextClient,
      public device::mojom::blink::DeviceServiceProviderClient {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit DeviceService(LocalDOMWindow*);
  ~DeviceService() override;

  // Web-exposed interfaces
  DEFINE_ATTRIBUTE_EVENT_LISTENER(change, kChange)
  String type();

  // EventTarget overrides.
  ExecutionContext* GetExecutionContext() const override;
  const AtomicString& InterfaceName() const override;

  void Trace(blink::Visitor*) const override;

 private:
  // DeviceServiceServiceClient
  void OnServiceChanged(
      device::mojom::blink::DeviceServiceType service) override;
  void AddedEventListener(const AtomicString& event_type,
                          RegisteredEventListener&) override;
  void OnServiceConnectionError();
  void EnsureServiceConnection();

  device::mojom::blink::DeviceServiceType Service_ =
      device::mojom::blink::DeviceServiceType::kCPUComputeCapacity;
  HeapMojoRemote<device::mojom::blink::DeviceServiceProvider> service_;
  HeapMojoReceiver<device::mojom::blink::DeviceServiceProviderClient,
                   DeviceService>
      receiver_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_SERVICE_DEVICE_SERVICE_H_
