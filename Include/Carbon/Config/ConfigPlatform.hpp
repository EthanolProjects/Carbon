#pragma once
#if defined (__linux__) || defined (linux) || defined (__linux)
#   define CARBON_TARGET_POSIX
#   define CARBON_TARGET_LINUX
#elif defined (__APPLE__)
#   define CARBON_TARGET_POSIX
#   define CARBON_TARGET_APPLE
#   if defined(__MACH__)
#       define CARBON_TARGET_MACOSX
#   else
#       define CARBON_TARGET_IOS
#   endif
#elif defined (_WIN32) || defined (_WIN64) || defined (__WIN32__) || defined (__TOS_WIN__) || defined (__WINDOWS__) || defined (__CYGWIN__)
#   define CARBON_TARGET_WINDOWS
#endif
