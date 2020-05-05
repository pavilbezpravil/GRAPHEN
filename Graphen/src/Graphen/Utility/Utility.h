#pragma once

#include "gnpch.h"

#ifndef SafeDelete
#define SafeDelete(ptr) if (ptr) { delete ptr; ptr = nullptr; }
#endif

#ifndef SafeRelease
#define SafeRelease(x) if (x != nullptr) { x->Release(); x = nullptr; }
#endif

void SIMDMemCopy( void* __restrict Dest, const void* __restrict Source, size_t NumQuadwords );
void SIMDMemFill( void* __restrict Dest, __m128 FillVector, size_t NumQuadwords );

std::wstring MakeWStr( const std::string& str );
std::string MakeStr( const std::wstring& str );

