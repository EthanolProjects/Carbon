#pragma once
#ifdef CARBON_TARGET_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#endif