#pragma once

#include <spdlog/spdlog.h>

#include <condition_variable>
#include <mutex>
#include <queue>

#include "trait.h"

namespace coro {
template <typename T>
class Queue {
 public:
  template <DecaySame<T> U>
  void Push(U &&);

  T &&Pop();

 private:
  std::queue<T> data_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

template <typename T>
template <DecaySame<T> U>
inline void Queue<T>::Push(U &&val) {
  std::lock_guard<std::mutex> lock_guard{mutex_};

  data_.push(std::forward<U>(val));
  cond_.notify_one();
}

template <typename T>
inline T &&Queue<T>::Pop() {
  std::unique_lock<std::mutex> lock{mutex_};
  cond_.wait(lock, [this] { return !data_.empty(); });
  auto ret = std::move(data_.front());
  data_.pop();

  return std::move(ret);
}

}  // namespace coro