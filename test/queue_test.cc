#include "queue.h"

#include <gtest/gtest.h>

TEST(Queue, Run) {
  coro::Queue<std::function<void()>> queue;
}
