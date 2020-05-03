#pragma once

#include <memory>
#include "Log.h"

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
		#define HZ_PLATFORM_WINDOWS
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define HZ_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define HZ_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
	#define HZ_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define HZ_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif // End of platform detection

#ifdef HZ_DEBUG
	#if defined(HZ_PLATFORM_WINDOWS)
		#define GN_DEBUGBREAK() __debugbreak()
	#elif defined(HZ_PLATFORM_LINUX)
		#include <signal.h>
		#define GN_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define GN_ENABLE_ASSERTS
#else
	#define GN_DEBUGBREAK()
#endif

#ifdef GN_ENABLE_ASSERTS
   #define GN_ASSERT(x) { if(!(x)) { GN_ERROR("Assertion Failed: {0}", #x); GN_DEBUGBREAK(); } }
   #define GN_ASSERT_MSG(x, ...) { if(!(x)) { GN_ERROR("Assertion Failed: {0}\nMessage: {1}", #x, __VA_ARGS__); GN_DEBUGBREAK(); } }
   #define GN_CORE_ASSERT(x) { if(!(x)) { GN_CORE_ERROR("Assertion Failed: {0}", #x); GN_DEBUGBREAK(); } }
   #define GN_CORE_ASSERT_MSG(x, ...) { if(!(x)) { GN_CORE_ERROR("Assertion Failed: {0}\nMessage: {1}", #x,__VA_ARGS__); GN_DEBUGBREAK(); } }
#else
	#define GN_ASSERT(x)
   #define GN_ASSERT_MSG(x, ...)
	#define GN_CORE_ASSERT(x, ...)
   #define GN_CORE_ASSERT_MSG(x, ...)
#endif

#define BIT(x) (1 << x)

#define GN_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
