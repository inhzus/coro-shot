#pragma once

#include <spdlog/spdlog.h>

#include <concepts>
#include <condition_variable>
#include <mutex>
#include <queue>

#include "trait.h"

namespace coro {
template <typename T>
class Queue {
 public:
  template <std::convertible_to<T> U>
  void Push(U &&) noexcept;

  T Pop() noexcept;
  template <typename Rep, typename Period>
  bool Pop(std::chrono::duration<Rep, Period> const &timeout, T *val) noexcept;

 private:
  std::queue<T> data_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

template <typename T>
template <std::convertible_to<T> U>
inline void Queue<T>::Push(U &&val) noexcept {
  std::lock_guard<std::mutex> lock_guard{mutex_};

  data_.push(T(std::forward<U>(val)));
  cond_.notify_one();
}

template <typename T>
template <typename Rep, typename Period>
inline bool Queue<T>::Pop(std::chrono::duration<Rep, Period> const &timeout,
                          T *val) noexcept {
  if (val == nullptr) {
    return false;
  }

  std::unique_lock<std::mutex> lock{mutex_};
  cond_.wait_for(lock, timeout, [this] { return !data_.empty(); });
  if (data_.empty()) {
    return false;
  }

  *val = ConstructWith(data_.front());
  return true;
}

template <typename T>
inline T Queue<T>::Pop() noexcept {
  std::unique_lock<std::mutex> lock{mutex_};
  cond_.wait(lock, [this] { return !data_.empty(); });
  auto ret = ConstructWith(data_.front());
  data_.pop();

  return ret;
}

}  // namespace coro