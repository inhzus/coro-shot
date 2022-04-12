#include <type_traits>

namespace coro {

template <typename T>
T ConstructWith(T &val) {
  if constexpr (std::is_move_constructible_v<T>) {
    return T{std::move(val)};
  } else if constexpr (std::is_copy_constructible_v<T>) {
    return T{val};
  } else {
    static_assert(!sizeof(T));
  }
}

}  // namespace coro
