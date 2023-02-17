// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/device_service/device_service.h"

#include "third_party/blink/public/common/browser_interface_broker_proxy.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"

namespace blink {

namespace {

String ServiceToString(device::mojom::blink::DeviceServiceType service) {
  switch (service) {
    case device::mojom::blink::DeviceServiceType::kCPUComputeCapacity:
      return "CPUComputeCapacity";
    case device::mojom::blink::DeviceServiceType::kGPUComputeCapacity:
      return "GPUComputeCapacity";
    case device::mojom::blink::DeviceServiceType::kVPUComputeCapacity:
      return "VPUComputeCapacity";
  }
}

}  // namespace

DeviceService::DeviceService(LocalDOMWindow* window)
    : ExecutionContextClient(window),
      service_(GetExecutionContext()),
      receiver_(this, GetExecutionContext()) {}

DeviceService::~DeviceService() = default;

String DeviceService::type() {
  EnsureServiceConnection();
  return ServiceToString(Service_);
}

void DeviceService::OnServiceChanged(
    device::mojom::blink::DeviceServiceType service) {
  if (Service_ == service)
    return;

  Service_ = service;
  DispatchEvent(*Event::CreateBubble(event_type_names::kChange));
}

void DeviceService::EnsureServiceConnection() {
  auto* context = GetExecutionContext();
  if (!context)
    return;

  if (service_.is_bound())
    return;

  auto task_runner =
      GetExecutionContext()->GetTaskRunner(TaskType::kMiscPlatformAPI);
  GetExecutionContext()->GetBrowserInterfaceBroker().GetInterface(
      service_.BindNewPipeAndPassReceiver(task_runner));

  service_->AddListenerAndGetCurrentService(
      receiver_.BindNewPipeAndPassRemote(task_runner),
      WTF::BindOnce(&DeviceService::OnServiceChanged, WrapPersistent(this)));
}

void DeviceService::AddedEventListener(const AtomicString& event_type,
                                       RegisteredEventListener& listener) {
  EventTargetWithInlineData::AddedEventListener(event_type, listener);

  if (event_type != event_type_names::kChange)
    return;

  ExecutionContext* context = GetExecutionContext();
  if (!context)
    return;

  EnsureServiceConnection();
}

ExecutionContext* DeviceService::GetExecutionContext() const {
  return ExecutionContextClient::GetExecutionContext();
}

const AtomicString& DeviceService::InterfaceName() const {
  return event_target_names::kDeviceService;
}

void DeviceService::Trace(blink::Visitor* visitor) const {
  visitor->Trace(service_);
  visitor->Trace(receiver_);
  EventTargetWithInlineData::Trace(visitor);
  ExecutionContextClient::Trace(visitor);
}

}  // namespace blink
