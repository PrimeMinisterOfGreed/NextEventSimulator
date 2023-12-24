#pragma once
#include <LogEngine.hpp>
#include <fmt/core.h>
#include <vector>

[[noreturn]] void _panic(const char *message);

void panic(const char *message, const char *file = __FILE_NAME__, int line = __LINE__);

void panic(std::string message, const char *file = __FILE_NAME__, int line = __LINE__);