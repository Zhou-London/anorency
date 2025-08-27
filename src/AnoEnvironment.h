#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>

#include "AnoInterface.h"
#include "AnoThread.h"
#include "Flags.h"

class AnoEnvironment {
 public:
  AnoEnvironment() {}

  template <typename FLAG>
  std::unique_ptr<AnoThread<FLAG>> introduce(
      std::function<FLAG(AnoInterface&)>) {
    return nullptr;
  }

  template <>
  std::unique_ptr<AnoThread<NewThread>> introduce(
      std::function<NewThread(AnoInterface&)> func) {
    const auto this_id = (uint32_t)(threads.size() + 1);

    AnoInterface interface(this_id);
    auto ano_thread =
        std::make_unique<AnoThread<NewThread>>(std::move((func)), this_id);

    threads[this_id] = std::thread([&]() { ano_thread->execute(interface); });

    threads[this_id].detach();

    return std::move(ano_thread);
  }

  void join(uint32_t id) { threads.at(id).join(); };

 private:
  std::unordered_map<uint32_t, std::thread> threads;
};