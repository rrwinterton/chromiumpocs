#include <iostream>
#include <emscripten.h>

#define USE_HWY 1

#ifdef USE_HWY

#include <hwy/highway.h>
namespace hwyn = hwy::HWY_NAMESPACE;

HWY_ALIGN int c[16];
using T = float;

void MulAddLoop(const T* HWY_RESTRICT mul_array,
                const T* HWY_RESTRICT add_array,
                const size_t size, T* HWY_RESTRICT x_array) {
  const hwyn::ScalableTag<T> d;
  for (size_t i = 0; i < size; i += hwyn::Lanes(d)) {
    const auto mul = hwyn::Load(d, mul_array + i);
    const auto add = hwyn::Load(d, add_array + i);
    auto x = hwyn::Load(d, x_array + i);
    x = hwyn::MulAdd(mul, x, add);
    hwyn::Store(x, d, x_array + i);
  }

}

#endif

extern "C" {

EMSCRIPTEN_KEEPALIVE int myAdd(int a, int b) {
    return a + b;
}

#ifdef USE_HWY    
EMSCRIPTEN_KEEPALIVE int pocHwy(int flag) {
  T mul_array[8];
  T add_array[8];
  T x_array[8];
  size_t size;
  size = 4;
  int a, b, c;
  c = myAdd(a, b);
  MulAddLoop(mul_array, add_array, size, x_array);
  return 1;
  }
#else
EMSCRIPTEN_KEEPALIVE int pocHwy(int flag) {
  return 0;
}
#endif  
}

