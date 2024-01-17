#pragma once
#define panic(message) _panic(message, __FILE_NAME__, __LINE__)
#ifdef CORE_DEBUG
#define core_assert(condition, failmessage, ...)                                                                       \
    if (!(condition))                                                                                                  \
        panic(fmt::format(failmessage, __VA_ARGS__));
#else
#define core_assert(condition, failmessage, args...)
#endif
#include <LogEngine.hpp>
#include <fmt/core.h>
#include <vector>

[[noreturn]] void _panic(const char *message);

void _panic(const char *message, const char *file, int line);

void _panic(std::string message, const char *file, int line);