#include <spdlog/spdlog.h>

#include "generator.h"

template <typename T>
coro::Generator<T> Generate(T begin, T end) {
  for (auto cur = std::move(begin); cur < end; ++cur) {
    co_yield cur;
  }
  co_return;
}

int main() {
  auto gen = Generate(0, 10);
  for (auto val : gen) {
    spdlog::info("val: {}", val);
  }
  return 0;
}
