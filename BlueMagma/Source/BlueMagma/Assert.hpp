#pragma once
#include <print>

#if _DEBUG
#define BM_ENABLE_ASSERT
#endif

#ifdef BM_ENABLE_ASSERT

#define BM_CORE_ASSERT(x) if (!(x)) { std::println("<BlueMagma> Assertion failed: '{}' [{} - {}:{}]", #x, __FUNCTION__, __FILE__, __LINE__); __debugbreak(); }
#define BM_ASSERT(x) if (!(x)) { std::println("<App> Assertion failed: '{}' [{} - {}:{}]", #x, __FUNCTION__, __FILE__, __LINE__); __debugbreak(); }

#endif