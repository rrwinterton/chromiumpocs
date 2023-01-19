// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/device_service/navigator_device_service.h"

#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/navigator.h"
#include "third_party/blink/renderer/modules/device_service/device_service.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"

namespace blink {

// static
const char NavigatorDeviceService::kSupplementName[] = "NavigatorDeviceService";

// static
DeviceService* NavigatorDeviceService::deviceService(Navigator& navigator) {
  DCHECK(RuntimeEnabledFeatures::DeviceServiceEnabled());

  NavigatorDeviceService* supplement =
      Supplement<Navigator>::From<NavigatorDeviceService>(navigator);
  if (!supplement) {
    supplement = MakeGarbageCollected<NavigatorDeviceService>(navigator);
    ProvideTo(navigator, supplement);
  }
  return supplement->service_;
}

NavigatorDeviceService::NavigatorDeviceService(Navigator& navigator)
    : Supplement<Navigator>(navigator),
      service_(MakeGarbageCollected<DeviceService>(
          GetSupplementable()->DomWindow())) {}

void NavigatorDeviceService::Trace(Visitor* visitor) const {
  visitor->Trace(service_);
  Supplement<Navigator>::Trace(visitor);
}

}  // namespace blink
