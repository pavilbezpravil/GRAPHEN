#pragma once

#include "wrl/client.h"
#include <memory>

#define ALIGN(n) __declspec(align(n))
#define GPU_ALIGN ALIGN(16)

using Microsoft::WRL::ComPtr;

template <typename T> using uptr = std::unique_ptr<T>;
template <typename T> using sptr = std::shared_ptr<T>;
template <typename T> using wptr = std::weak_ptr<T>;

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