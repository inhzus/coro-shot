#include "queue.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

TEST(Queue, Run) {
  coro::Queue<int> queue;
  std::jthread t{[&queue] {
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    queue.Push(5);
  }};
  auto top = queue.Pop();
  ASSERT_EQ(5, top);
}

TEST(Queue, PopWithTimeout) {
  coro::Queue<int> queue;
  std::jthread t{[&queue] {
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    queue.Push(5);
  }};

  int val{};
  bool ok = queue.Pop(std::chrono::milliseconds{0}, &val);
  ASSERT_FALSE(ok);
}
