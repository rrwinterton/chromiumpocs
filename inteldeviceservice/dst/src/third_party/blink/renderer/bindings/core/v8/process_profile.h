
#ifndef THIRD_PARTY_BLINK_RENDERER_CONTROLLER_PROCESS_PROFILE_H_
#define THIRD_PARTY_BLINK_RENDERER_CONTROLLER_PROCESS_PROFILE_H_

#include "third_party/blink/renderer/platform//wtf/allocator/allocator.h"
#include "v8/include/v8.h"

class ProcessProfiler {
  STATIC_ONLY(ProcessProfiler);

 public:
  static void Initialize(v8::Isolate* isolate);
};

#endif //THIRD_PARTY_BLINK_RENDERER_CONTROLLER_PROCESS_PROFILE_H_