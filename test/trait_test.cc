#include "trait.h"

#include <gtest/gtest.h>

namespace {

class Foo {
 public:
  Foo(std::string value_) noexcept : value{std::move(value_)} {}

  explicit Foo(const Foo &rhs) noexcept : value{rhs.value}, copied_{true} {}
  explicit Foo(Foo &&rhs) noexcept
      : value{std::move(rhs.value)}, moved_{true} {}

  Foo &operator=(const Foo &rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    value = rhs.value;
    copied_ = true;
    return *this;
  }

  Foo &operator=(Foo &&rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    value = std::move(rhs.value);
    moved_ = true;
    return *this;
  }

  constexpr bool copied() const noexcept { return copied_; }
  constexpr bool moved() const noexcept { return moved_; }

  std::string value;

 private:
  bool copied_{false};
  bool moved_{false};
};

class Bar : public Foo {
 public:
  using Foo::Foo;

  Bar(const Bar &rhs) noexcept : Foo{rhs} {}
  Bar &operator=(const Bar &rhs) noexcept {
    Foo::operator=(rhs);
    return *this;
  }

  Bar(Bar &&) = delete;
  Bar &operator=(Bar &&) = delete;
};

}  // namespace

TEST(Trait, ConstructWithMoveConstructor) {
  const std::string expected = "fake";
  Foo foo{expected};

  auto res = coro::ConstructWith(foo);
  EXPECT_EQ(res.value, expected);
  ASSERT_TRUE(res.moved());
}

TEST(Trait, ConstructWithCopyConstructor) {
  const std::string expected = "fake";
  Bar bar{expected};

  auto res = coro::ConstructWith(bar);
  EXPECT_EQ(res.value, expected);
  ASSERT_TRUE(res.copied());
}
