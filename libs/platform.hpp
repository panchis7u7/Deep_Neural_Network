// Redefining types to its corresponding byte size.
///////////////////////////////////////////////////

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
typedef float f32;
typedef double f64;
typedef int bool32;
typedef char bool8;
#define TRUE 1
#define FALSE 0

#if defined(WIN32) || defined(_WIN64) || defined(_WIN32) || defined(__WIN32__)
#define WINDOWS_PLATFORM TRUE
#ifndef _WIN64
#error "64 Bit Windows is required!"
#endif //_WIN64

// Linux based systems.
#elif defined(__linux__) || defined(__gnu_linux__)
#define LINUX_PLATFORM TRUE
#ifdef __ANDROID__
#define ANDROID_PLATFORM TRUE
#endif //__ANDROID__
#elif __unix__
#define UNIX_PLATFORM TRUE
#elif _POSIX_VERION
#define POSIX_PLATFORM TRUE
#elif __APPLE__
// Apple platform.
#define APPLE_PLATFORM TRUE
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// IOS Simulator.
#define IOS_PLATFORM TRUE
#define IOS_SIMULATOR_PLATFORM TRUE
#elif TARGET_OS_PHONE
#define IOS_PLATFORM TRUE
// IOS Device.
#elif TARGET_OS_MAC
// Other kind of MAC OS.
#else
#error "Unknown apple platform!"
#endif
#else
#error "Unknown platform!"
#endif

#ifdef VKCEXPORT
#ifdef _MSC_VER
#define LIBEXP __declspec(dllexport)
#else
#define LIBEXP __attribute__((visibility("default")))
#endif
#else
// Imports.
#ifdef _MSC_VER
#define LIBEXP __declspec(dllimport)
#else
#define LIBEXP
#endif
#endif