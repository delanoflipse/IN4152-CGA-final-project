#pragma once
#include <chrono>
#include <ctime>

namespace timing
{
  std::chrono::system_clock::time_point start_time;
  std::chrono::system_clock::time_point last_time;

  float time_s;
  float delta_s;

  void start() {
    start_time = std::chrono::system_clock::now();
    last_time = start_time;
  };

  void update()
  {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<float> time_since = now - start_time;
    std::chrono::duration<float> delta_since = now - last_time;
    time_s = time_since.count();
    delta_s = delta_since.count();

    last_time = now;
  };
};