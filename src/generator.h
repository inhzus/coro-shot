#pragma once
#include <exception>
#include <optional>

#include "config.h"

namespace coro {

template <typename T>
class Generator {
 public:
  struct Promise;
  using promise_type = Promise;

  struct Promise {
   public:
    Generator get_return_object() noexcept {
      return Generator{tr::coroutine_handle<Promise>::from_promise(*this)};
    }

    constexpr tr::suspend_always initial_suspend() noexcept { return {}; }
    constexpr tr::suspend_always final_suspend() noexcept { return {}; }
    tr::suspend_always yield_value(const T &val) noexcept {
      val_ = val;
      return {};
    }
    tr::suspend_always yield_value(T &&val) noexcept {
      val_ = std::move(val);
      return {};
    }
    constexpr void return_void() noexcept {}
    void unhandled_exception() noexcept { ex_ = std::current_exception(); }

    const T &Value() const { return val_.value(); }
    T &Value() { return val_.value(); }
    void RethrowIfNeed() {
      if (ex_) {
        std::rethrow_exception(ex_);
      }
    }

    template <typename U>
    tr::suspend_never await_transform(U &&) = delete;

    std::optional<T> val_;
    std::exception_ptr ex_;
  };

  explicit Generator(tr::coroutine_handle<Promise> handle)
      : handle_{handle} {}
  auto handle() noexcept { return handle_; }

  class IteratorSentinel {};

  class Iterator {
   public:
    explicit Iterator(tr::coroutine_handle<Promise> *handle) : h_{handle} {}

    const T &operator*() const { return h_->promise().Value(); }
    T &operator*() { return h_->promise().Value(); }
    const T *operator->() const { return &h_->promise().Value(); }
    T *operator->() { return &h_->promise().Value(); }

    const T &operator++() {
      h_->resume();
      if (h_->done()) {
        h_->promise().RethrowIfNeed();
      }
      return operator*();
    }

    T operator++(int) {
      auto prev = operator*();
      operator++();
      return prev;
    }

    bool operator==(const IteratorSentinel &) const { return h_->done(); }
    bool operator!=(const IteratorSentinel &rhs) const {
      return !operator==(rhs);
    }

   private:
    tr::coroutine_handle<Promise> *h_;
  };

  Iterator begin() {
    handle_.resume();
    if (handle_.done()) {
      handle_.promise().RethrowIfNeed();
    }
    return Iterator{&handle_};
  }
  IteratorSentinel end() { return {}; }

 private:
  tr::coroutine_handle<Promise> handle_;
};

}  // namespace coro