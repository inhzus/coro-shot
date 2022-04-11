#pragma once
#include <functional>
#include <thread>
#include <vector>

#include "queue.h"

namespace coro {

class ThreadPool {
 public:
  explicit ThreadPool(size_t thread_num) noexcept;

  void Add(std::function<void()> task) noexcept;

 private:
  void ThreadRun() noexcept;

  bool stop_{false};
  std::vector<std::thread> threads_;
  Queue<std::function<void()>> queue_;
};

ThreadPool::ThreadPool(size_t thread_num) noexcept {
  for (int i = 0; i < thread_num; ++i) {
    threads_.emplace_back([this] { ThreadRun(); });
  }
}

void ThreadPool::ThreadRun() noexcept {
}

}  // namespace coro
