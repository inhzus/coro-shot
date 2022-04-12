#pragma once

#include <variant>

#include "config.h"

namespace coro {

template <typename T = void>
class Task {
 public:
  class Promise;
  using promise_type = Promise;

  class Promise {
   public:
    friend struct FinalAwaitable;
    struct FinalAwaitable {
      constexpr bool await_ready() const noexcept { return false; }
      tr::coroutine_handle<> await_suspend(
          tr::coroutine_handle<Promise> handle) noexcept {
        return handle.promise().continuation_;
      }
      constexpr void await_resume() noexcept {}
    };

    auto initial_suspend() noexcept { return tr::suspend_always{}; }
    auto final_suspend() noexcept { return FinalAwaitable{}; }
    void set_contiuation(tr::coroutine_handle<> handle) noexcept {
      continuation_ = handle;
    }

    Task<T> get_return_object() noexcept {
      return Task<T>{tr::coroutine_handle<Promise>::from_promise{*this}};
    }
    void unhandled_exception() noexcept { ex_ = std::current_exception(); }

   private:
    tr::coroutine_handle<> continuation_;
    std::optional<T> value_;
    std::exception_ptr ex_;
  };

 private:
};

}  // namespace coro