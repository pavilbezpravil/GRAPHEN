#pragma once

#include "wrl/client.h"
#include <memory>

#define ALIGN(n) __declspec(align(n))
#define GPU_ALIGN ALIGN(16)

using Microsoft::WRL::ComPtr;

template <typename T> using uptr = std::unique_ptr<T>;
template <typename T> using sptr = std::shared_ptr<T>;
template <typename T> using wptr = std::weak_ptr<T>;
