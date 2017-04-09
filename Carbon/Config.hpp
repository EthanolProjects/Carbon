#pragma once
#if defined(_WIN32) || defined(__CYGWIN__)
#    ifdef __GNUC__
#        define EXPORT __attribute__ ((dllexport))
#        define IMPORT __attribute__ ((dllimport))
#    else
#        define EXPORT __declspec(dllexport) 
#        define IMPORT __declspec(dllimport)
#    endif
#else
#    define EXPORT __attribute__ ((visibility ("default")))
#    define IMPORT
#endif

#ifdef CARBON_EXPORTS
#define CARBON_API EXPORT
#else
#define CARBON_API IMPORT
#endif
