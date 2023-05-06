
#include "third_party/blink/renderer/bindings/core/v8/process_profile.h"

#include <windows.h>

#include "base/threading/sequence_local_storage_slot.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/device/public/mojom/device_service_provider.mojom-blink.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/platform.h"

namespace {
device::mojom::blink::DeviceServiceProvider& Provider() {
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
}  // namespace

void ProcessProfiler::Initialize(v8::Isolate* isolate) {
  auto &provider = Provider();
  provider.StartCollectingLbr(::GetCurrentProcessId(), ::GetCurrentThreadId());

  // TODO: Hook isolate to provider
}
