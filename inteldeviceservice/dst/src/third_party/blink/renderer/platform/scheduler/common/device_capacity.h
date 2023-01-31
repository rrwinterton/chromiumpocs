#include <string>
#include <map>

#define WEIGHT_DECAY 0.15

class Device_Capacity {
 public:
  Device_Capacity() noexcept;
  ~Device_Capacity() noexcept;
  void setCapacity(int id, float currentCapacity);
  void getWeightedCapacity(int id, float& Capacity);
  void getHighCapacity(int id, float& Capacity);

 private:
  std::map<int, float> highCapacity;
  std::map<int, float> weightedCapacity;
};
