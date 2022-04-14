#pragma once

#include <exception>
#include <optional>
#include <stdexcept>
#include <variant>

#include "config.h"

namespace coro {

template <typename>
class Task;

namespace detail {

template <typename T>
class PromiseBase {
 public:
  friend struct FinalAwaitable;
  struct FinalAwaitable {
    [[nodiscard]] constexpr bool await_ready() const noexcept { return false; }
    template <typename P>
    requires std::is_base_of_v<PromiseBase, P> tr::coroutine_handle<>
    await_suspend(tr::coroutine_handle<P> handle)
    noexcept { return handle.promise().continuation_; }
    constexpr void await_resume() noexcept {}
  };

  auto initial_suspend() noexcept { return tr::suspend_always{}; }
  auto final_suspend() noexcept { return FinalAwaitable{}; }
  void set_continuation(tr::coroutine_handle<> handle) noexcept {
    continuation_ = handle;
  }

  void unhandled_exception() noexcept { ex_ = std::current_exception(); }

  Task<T> get_return_object() noexcept;

 protected:
  tr::coroutine_handle<> continuation_;
  std::exception_ptr ex_;
};

template <typename T>
class Promise : public PromiseBase<T> {
 public:
  void return_value(T &&val) { val_ = std::move(val); }
  void return_value(const T &val) { val_ = val; }

  T &Value() & {
    if (PromiseBase<T>::ex_ != nullptr) {
      std::rethrow_exception(PromiseBase<T>::ex_);
    }
    return val_.value();
  }

  T Value() && {
    if (PromiseBase<T>::ex_ != nullptr) {
      std::rethrow_exception(PromiseBase<T>::ex_);
    }

    return std::move(val_.value());
  }

 protected:
  std::optional<T> val_;
};

template <>
class Promise<void> : public PromiseBase<void> {
 public:
  void return_void() {}
  void Value() {
    if (PromiseBase::ex_ != nullptr) {
      std::rethrow_exception(PromiseBase::ex_);
    }
  }
};

}  // namespace detail

template <typename T = void>
class Task {
 public:
  using promise_type = detail::Promise<T>;
  using value_type = T;

  Task() noexcept : handle_{nullptr} {}
  explicit Task(tr::coroutine_handle<promise_type> handle) : handle_{handle} {}
  ~Task() {
    if (handle_) handle_.destroy();
  }
  Task(Task &&rhs) noexcept : handle_{rhs.handle_} { rhs.handle_ = nullptr; }

  struct AwaitableBase {
    tr::coroutine_handle<promise_type> handle;
    explicit AwaitableBase(tr::coroutine_handle<promise_type> handle_) noexcept
        : handle(handle_) {}

    constexpr bool await_ready() const noexcept {
      return handle == nullptr || handle.done();
    }
    tr::coroutine_handle<> await_suspend(
        tr::coroutine_handle<> awaiting) noexcept {
      handle.promise().set_continuation(awaiting);
      return handle;
    }
  };

  auto operator co_await() const &noexcept {
    struct Awaitable : AwaitableBase {
      using AwaitableBase::AwaitableBase;
      decltype(auto) await_resume() {
        if (this->handle == nullptr) {
          throw std::runtime_error("");
        }

        return this->handle.promise().Value();
      }
    };
    return Awaitable{handle_};
  }

  auto operator co_await() &&noexcept {
    struct Awaitable : AwaitableBase {
      using AwaitableBase::AwaitableBase;
      decltype(auto) await_resume() {
        if (this->handle == nullptr) {
          throw std::runtime_error("");
        }

        return std::move(this->handle.promise()).Value();
      }
    };
    return Awaitable{handle_};
  }

  auto &handle() noexcept { return handle_; }

 private:
  tr::coroutine_handle<promise_type> handle_;
};

namespace detail {

template <typename T>
Task<T> PromiseBase<T>::get_return_object() noexcept {
  return Task<T>{tr::coroutine_handle<Promise<T>>::from_promise(
      static_cast<Promise<T> &>(*this))};
}

}  // namespace detail

}  // namespace coro