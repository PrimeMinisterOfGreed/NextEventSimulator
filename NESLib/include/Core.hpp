#pragma once
#include <LogEngine.hpp>
#include <fmt/core.h>
#include <vector>
#define panic(message) _panic(message, __FILE_NAME__, __LINE__)

[[noreturn]] void _panic(const char *message);

void _panic(const char *message, const char *file, int line);

void _panic(std::string message, const char *file, int line);