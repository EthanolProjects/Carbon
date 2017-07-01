#pragma once
#ifdef CARBON_TARGET_WINDOWS
// Set WinNT Version to 6
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0620
// Disable useless info
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
// Import Windows defs
#include <Windows.h>
#else
#endif
