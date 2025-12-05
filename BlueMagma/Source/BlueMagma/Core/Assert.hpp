#pragma once
#include "Log.hpp"

#if _DEBUG
#define BM_ENABLE_ASSERT
#endif

#ifdef BM_ENABLE_ASSERT

#define BM_ASSERT_IMPL(type, x, ...) \
	if (!(x)) { \
		BM##type##CRITICAL("Assert '{}' failed [{} - {}:{}]{}", #x, __FUNCTION__, BM_FILENAME, __LINE__, __VA_ARGS__); \
		__debugbreak(); }

#define BM_ASSERT_IMPL_MSG(type, x, ...)	BM_ASSERT_IMPL(type, x, "\n - " __VA_ARGS__)
#define BM_ASSERT_IMPL_NOMSG(type, x, ...)	BM_ASSERT_IMPL(type, x, "")

#define BM_ASSERT_IMPL_EXPAND(macro)	macro

#define BM_ASSERT_IMPL_SELECT_MACRO(_1, _2, macro, ...)	macro
#define BM_ASSERT_IMPL_GET_MACRO(...)	BM_ASSERT_IMPL_EXPAND(BM_ASSERT_IMPL_SELECT_MACRO(__VA_ARGS__, BM_ASSERT_IMPL_MSG, BM_ASSERT_IMPL_NOMSG))

#define BM_CORE_ASSERT(...)  BM_ASSERT_IMPL_EXPAND(BM_ASSERT_IMPL_GET_MACRO(__VA_ARGS__)##(_CORE_, __VA_ARGS__))
#define BM_ASSERT(...)       BM_ASSERT_IMPL_EXPAND(BM_ASSERT_IMPL_GET_MACRO(__VA_ARGS__)##(_, __VA_ARGS__))

#else

#define BM_CORE_ASSERT(...)
#define BM_ASSERT(...)

#endif