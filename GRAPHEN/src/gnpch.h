#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Graphen/Core/Log.h"
#include "Graphen/Core/Hash.h"

#include "Graphen/Debug/Instrumentor.h"

#ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
   #define NOMINMAX
#endif
#include <windows.h>

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

#include <d3d12.h>
#include <D3Dcompiler.h>
#include "Graphen/Render/d3dx12.h"
#include <wrl.h>
#include <ppltasks.h>

#include "Graphen/Core/Common.h"

#include "Graphen/Utility/Utility.h"
#include "Graphen/Math/VectorMath.h"

#ifdef HZ_PLATFORM_WINDOWS
	// #include <Windows.h>
#endif