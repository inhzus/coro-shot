#include <type_traits>

namespace coro {

template <typename T, typename U>
concept DecaySame = std::is_same_v<std::decay_t<T>, std::decay_t<U>>;

}
