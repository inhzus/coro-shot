//
// Created by suun on 4/14/22.
//

#include "task.h"

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <thread>

namespace {

coro::Task<void> First() {
  std::this_thread::sleep_for(std::chrono::seconds{1});
  spdlog::info("First");
  co_return;
}

coro::Task<void> Second() {
  auto t = First();
  std::this_thread::sleep_for(std::chrono::seconds{1});
  co_await t;
  spdlog::info("Second");
  co_return;
}

TEST(TaskTest, Run) {
  spdlog::info("Start");
  auto task = Second();
  auto &handle = task.handle();
  handle.resume();
}

}  // namespace
