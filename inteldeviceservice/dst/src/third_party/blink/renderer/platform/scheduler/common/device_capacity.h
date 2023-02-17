#include <string>
#include <map>

#define WEIGHT_DECAY 0.15

class Device_Capacity {
 public:
  Device_Capacity() noexcept;
  ~Device_Capacity() noexcept;
  void setCapacity(uint32_t id, float currentCapacity);
  void getWeightedCapacity(uint32_t id, float& Capacity);
  void getHighCapacity(uint32_t id, float& Capacity);

 private:
  std::map<uint32_t, float> highCapacity;
  std::map<uint32_t, float> weightedCapacity;
};
