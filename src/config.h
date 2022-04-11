//
// Created by suun on 2022/4/11.
// Copyright (c) 2022 Alibaba. All rights reserved.
//

#pragma once

// Standard library support for coroutines.
#if defined(ASIO_MSVC)
#if (_MSC_VER >= 1928) && (_MSVC_LANG >= 201705)
#define CORO_HAS_STD_COROUTINE 1
#endif  // (_MSC_VER >= 1928) && (_MSVC_LANG >= 201705)
#endif  // defined(ASIO_MSVC)
#if defined(__GNUC__)
#if (__cplusplus >= 201709) && (__cpp_impl_coroutine >= 201902)
#if __has_include(<coroutine>)
#define CORO_HAS_STD_COROUTINE 1
#endif  // __has_include(<coroutine>)
#endif  // (__cplusplus >= 201709) && (__cpp_impl_coroutine >= 201902)
#endif  // defined(__GNUC__)

#ifndef CORO_HAS_STD_COROUTINE
#define CORO_HAS_STD_COROUTINE 0
#endif

#if CORO_HAS_STD_COROUTINE
#include <coroutine>
#else
#include <experimental/coroutine>
#endif

namespace coro::detail {

#if CORO_HAS_STD_COROUTINE
using std::coroutine_handle;
using std::suspend_always;
using std::suspend_never;
#else
using std::experimental::coroutine_handle;
using std::experimental::suspend_always;
using std::experimental::suspend_never;

#endif

}  // namespace coro::detail
