#pragma once

#ifdef GN_DEBUG
   #if defined(GN_PLATFORM_WINDOWS)
      #define HZ_DEBUGBREAK() __debugbreak()
   #else
      #error "Platform doesn't support debugbreak yet!"
   #endif
   #define GN_ENABLE_ASSERTS
#else
   #define GN_DEBUGBREAK()
#endif

#ifdef GN_ENABLE_ASSERTS
   #define GN_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK(); } }
   #define GN_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK(); } }
#else
   #define GN_ASSERT(x, ...)
   #define GN_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define GN_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace gn {

   template<typename T>
   using Scope = std::unique_ptr<T>;
   template<typename T, typename ... Args>
   constexpr Scope<T> CreateScope(Args&& ... args)
   {
      return std::make_unique<T>(std::forward<Args>(args)...);
   }

   template<typename T>
   using Ref = std::shared_ptr<T>;
   template<typename T, typename ... Args>
   constexpr Ref<T> CreateRef(Args&& ... args)
   {
      return std::make_shared<T>(std::forward<Args>(args)...);
   }

}
