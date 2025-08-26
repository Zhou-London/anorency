

#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <thread>

class AnoKernel {
 public:
  AnoKernel() : n(0) {}
  ~AnoKernel() {}

  uint32_t introduce_func(std::function<uint32_t(AnoKernel &)> &&func) {
    std::thread t([&]() { func(*this); });
    t.detach();
    return 0;
  }

  uint16_t n;
};

int main() {
  AnoKernel kern;

  kern.introduce_func([&](AnoKernel &ano_kern) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << (int)ano_kern.n << std::endl;

    return 1;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(1500));

  return 0;
}