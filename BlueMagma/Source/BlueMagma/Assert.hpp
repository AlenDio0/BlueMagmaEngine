#pragma once
#include "Log.hpp"

#if _DEBUG
#define BM_ENABLE_ASSERT
#endif

#ifdef BM_ENABLE_ASSERT

#define BM_CORE_ASSERT(x) if (!(x)) { BM_CORE_CRITICAL("Assert failed: '{}' [{} - {}:{}]", #x, __FUNCTION__, __FILE__, __LINE__); __debugbreak(); }
#define BM_ASSERT(x) if (!(x)) { BM_CRITICAL("Assert failed: '{}' [{} - {}:{}]", #x, __FUNCTION__, __FILE__, __LINE__); __debugbreak(); }

#else

#define BM_CORE_ASSERT(...)
#define BM_ASSERT(...)

#endif