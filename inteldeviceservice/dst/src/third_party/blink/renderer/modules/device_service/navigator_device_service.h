// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_SERVICE_NAVIGATOR_DEVICE_SERVICE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_SERVICE_NAVIGATOR_DEVICE_SERVICE_H_

#include "third_party/blink/renderer/core/frame/navigator.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/supplementable.h"

namespace blink {

class DeviceService;

class NavigatorDeviceService final
    : public GarbageCollected<NavigatorDeviceService>,
      public Supplement<Navigator> {
 public:
  static const char kSupplementName[];
  static DeviceService* deviceService(Navigator&);

  explicit NavigatorDeviceService(Navigator&);

  void Trace(Visitor*) const override;

 private:
  Member<DeviceService> service_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_SERVICE_NAVIGATOR_DEVICE_SERVICE_H_
