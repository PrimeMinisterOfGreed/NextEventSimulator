/**
 * @file Core.hpp
 * @author matteo ielacqua (you@domain.com)
 * @brief il file contiene alcune utilities utili per implementare una sorta di panic behaviour nel compilatore
 * tendenzialmente è una tecnica usata nei sistemi embedded per permettere allo sviluppatore di collegare un 
 * debugger e osservare lo stato del programma quando il fault è stato generato
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "LogEngine.hpp"

[[noreturn]] void _panic(const char *message);

void _panic(const char *message, const char *file, int line);

void _panic(std::string message, const char *file, int line);

#define panic(message) _panic(message, __FILE_NAME__, __LINE__)
#ifdef CORE_DEBUG
#define core_assert(condition,fmtline ...) \
if(!(condition))\
panic(fmt::format(fmtline));
#else
#define core_assert(condition, failmessage, ...)
#endif