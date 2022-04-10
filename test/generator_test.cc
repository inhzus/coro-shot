#include "generator.h"

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

template <typename T>
coro::Generator<T> Generate(T begin, T end) {
  for (auto cur = std::move(begin); cur < end; ++cur) {
    co_yield cur;
  }
  co_return;
}

TEST(Generator, Run) {
  auto gen = Generate(0, 10);
  for (auto val : gen) {
    spdlog::info("val: {}", val);
  }
}
