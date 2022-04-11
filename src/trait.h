#include <type_traits>

namespace coro {

template <typename T>
T ConstructWith(T &val) {
  if (std::is_move_constructible_v<T>) {
    return T{std::move(val)};
  } else if (std::is_copy_constructible_v<T>) {
    return T{val};
  } else {
    static_assert(std::is_move_constructible_v<T> ||
                  std::is_copy_constructible_v<T>);
  }
}

}  // namespace coro
