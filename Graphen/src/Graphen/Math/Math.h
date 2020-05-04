//-------------------------------------------------------------------------------------
// Math.h -- Simplified C++ Math wrapper for DirectXMath
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#pragma once

#if !defined(__d3d11_h__) && !defined(__d3d11_x_h__) && !defined(__d3d12_h__) && !defined(__d3d12_x_h__) && !defined(__XBOX_D3D12_X__)
#error include d3d11.h or d3d12.h before including Math.h
#endif

#if !defined(_XBOX_ONE) || !defined(_TITLE)
#include <dxgi1_2.h>
#endif

#include <functional>

#include <assert.h>
#include <memory.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include "Common.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif

 namespace Math
 {

    using namespace DirectX;

    struct Vector2;
     struct Vector4;
     struct Matrix4;
     struct Quaternion;
     struct Plane;

     //------------------------------------------------------------------------------
     // 2D rectangle
     struct Rectangle
     {
         long x;
         long y;
         long width;
         long height;

         // Creators
         Rectangle() noexcept : x(0), y(0), width(0), height(0) {}
         constexpr Rectangle(long ix, long iy, long iw, long ih) noexcept : x(ix), y(iy), width(iw), height(ih) {}
         explicit Rectangle(const RECT& rct) noexcept : x(rct.left), y(rct.top), width(rct.right - rct.left), height(rct.bottom - rct.top) {}

         Rectangle(const Rectangle&) = default;
         Rectangle& operator=(const Rectangle&) = default;

         Rectangle(Rectangle&&) = default;
         Rectangle& operator=(Rectangle&&) = default;

         operator RECT() noexcept { RECT rct; rct.left = x; rct.top = y; rct.right = (x + width); rct.bottom = (y + height); return rct; }
     #ifdef __cplusplus_winrt
         operator Windows::Foundation::Rect() noexcept { return Windows::Foundation::Rect(float(x), float(y), float(width), float(height)); }
     #endif

         // Comparison operators
         bool operator == (const Rectangle& r) const noexcept { return (x == r.x) && (y == r.y) && (width == r.width) && (height == r.height); }
         bool operator == (const RECT& rct) const noexcept { return (x == rct.left) && (y == rct.top) && (width == (rct.right - rct.left)) && (height == (rct.bottom - rct.top)); }

         bool operator != (const Rectangle& r) const noexcept { return (x != r.x) || (y != r.y) || (width != r.width) || (height != r.height); }
         bool operator != (const RECT& rct) const noexcept { return (x != rct.left) || (y != rct.top) || (width != (rct.right - rct.left)) || (height != (rct.bottom - rct.top)); }

         // Assignment operators
         Rectangle& operator=(_In_ const RECT& rct) noexcept { x = rct.left; y = rct.top; width = (rct.right - rct.left); height = (rct.bottom - rct.top); return *this; }

         // Rectangle operations
         Vector2 Location() const noexcept;
         Vector2 Center() const noexcept;

         bool IsEmpty() const noexcept { return (width == 0 && height == 0 && x == 0 && y == 0); }

         bool Contains(long ix, long iy) const noexcept { return (x <= ix) && (ix < (x + width)) && (y <= iy) && (iy < (y + height)); }
         bool Contains(const Vector2& point) const noexcept;
         bool Contains(const Rectangle& r) const noexcept { return (x <= r.x) && ((r.x + r.width) <= (x + width)) && (y <= r.y) && ((r.y + r.height) <= (y + height)); }
         bool Contains(const RECT& rct) const noexcept { return (x <= rct.left) && (rct.right <= (x + width)) && (y <= rct.top) && (rct.bottom <= (y + height)); }

         void Inflate(long horizAmount, long vertAmount) noexcept;

         bool Intersects(const Rectangle& r) const noexcept { return (r.x < (x + width)) && (x < (r.x + r.width)) && (r.y < (y + height)) && (y < (r.y + r.height)); }
         bool Intersects(const RECT& rct) const noexcept { return (rct.left < (x + width)) && (x < rct.right) && (rct.top < (y + height)) && (y < rct.bottom); }

         void Offset(long ox, long oy) noexcept { x += ox; y += oy; }

         // Static functions
         static Rectangle Intersect(const Rectangle& ra, const Rectangle& rb) noexcept;
         static RECT Intersect(const RECT& rcta, const RECT& rctb) noexcept;

         static Rectangle Union(const Rectangle& ra, const Rectangle& rb) noexcept;
         static RECT Union(const RECT& rcta, const RECT& rctb) noexcept;
     };

     //------------------------------------------------------------------------------
     // 2D vector
     struct Vector2 : public XMFLOAT2
     {
         Vector2() noexcept : XMFLOAT2(0.f, 0.f) {}
         constexpr explicit Vector2(float ix) noexcept : XMFLOAT2(ix, ix) {}
         constexpr Vector2(float ix, float iy) noexcept : XMFLOAT2(ix, iy) {}
         explicit Vector2(_In_reads_(2) const float *pArray) noexcept : XMFLOAT2(pArray) {}
         Vector2(FXMVECTOR V) noexcept { XMStoreFloat2(this, V); }
         Vector2(const XMFLOAT2& V) noexcept { this->x = V.x; this->y = V.y; }
         explicit Vector2(const XMVECTORF32& F) noexcept { this->x = F.f[0]; this->y = F.f[1]; }

         Vector2(const Vector2&) = default;
         Vector2& operator=(const Vector2&) = default;

         Vector2(Vector2&&) = default;
         Vector2& operator=(Vector2&&) = default;

         operator XMVECTOR() const noexcept { return XMLoadFloat2(this); }

         // Comparison operators
         bool operator == (const Vector2& V) const noexcept;
         bool operator != (const Vector2& V) const noexcept;

         // Assignment operators
         Vector2& operator= (const XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; return *this; }
         Vector2& operator+= (const Vector2& V) noexcept;
         Vector2& operator-= (const Vector2& V) noexcept;
         Vector2& operator*= (const Vector2& V) noexcept;
         Vector2& operator*= (float S) noexcept;
         Vector2& operator/= (float S) noexcept;

         // Unary operators
         Vector2 operator+ () const noexcept { return *this; }
         Vector2 operator- () const noexcept { return Vector2(-x, -y); }

         // Vector operations
         bool InBounds(const Vector2& Bounds) const noexcept;

         float Length() const noexcept;
         float LengthSquared() const noexcept;

         float Dot(const Vector2& V) const noexcept;
         void Cross(const Vector2& V, Vector2& result) const noexcept;
         Vector2 Cross(const Vector2& V) const noexcept;

         void Normalize() noexcept;
         void Normalize(Vector2& result) const noexcept;

         void Clamp(const Vector2& vmin, const Vector2& vmax) noexcept;
         void Clamp(const Vector2& vmin, const Vector2& vmax, Vector2& result) const noexcept;

         // Static functions
         static float Distance(const Vector2& v1, const Vector2& v2) noexcept;
         static float DistanceSquared(const Vector2& v1, const Vector2& v2) noexcept;

         static void Min(const Vector2& v1, const Vector2& v2, Vector2& result) noexcept;
         static Vector2 Min(const Vector2& v1, const Vector2& v2) noexcept;

         static void Max(const Vector2& v1, const Vector2& v2, Vector2& result) noexcept;
         static Vector2 Max(const Vector2& v1, const Vector2& v2) noexcept;

         static void Lerp(const Vector2& v1, const Vector2& v2, float t, Vector2& result) noexcept;
         static Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t) noexcept;

         static void SmoothStep(const Vector2& v1, const Vector2& v2, float t, Vector2& result) noexcept;
         static Vector2 SmoothStep(const Vector2& v1, const Vector2& v2, float t) noexcept;

         static void Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g, Vector2& result) noexcept;
         static Vector2 Barycentric(const Vector2& v1, const Vector2& v2, const Vector2& v3, float f, float g) noexcept;

         static void CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t, Vector2& result) noexcept;
         static Vector2 CatmullRom(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4, float t) noexcept;

         static void Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t, Vector2& result) noexcept;
         static Vector2 Hermite(const Vector2& v1, const Vector2& t1, const Vector2& v2, const Vector2& t2, float t) noexcept;

         static void Reflect(const Vector2& ivec, const Vector2& nvec, Vector2& result) noexcept;
         static Vector2 Reflect(const Vector2& ivec, const Vector2& nvec) noexcept;

         static void Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex, Vector2& result) noexcept;
         static Vector2 Refract(const Vector2& ivec, const Vector2& nvec, float refractionIndex) noexcept;

         static void Transform(const Vector2& v, const Quaternion& quat, Vector2& result) noexcept;
         static Vector2 Transform(const Vector2& v, const Quaternion& quat) noexcept;

         static void Transform(const Vector2& v, const Matrix4& m, Vector2& result) noexcept;
         static Vector2 Transform(const Vector2& v, const Matrix4& m) noexcept;
         static void Transform(_In_reads_(count) const Vector2* varray, size_t count, const Matrix4& m, _Out_writes_(count) Vector2* resultArray) noexcept;

         static void Transform(const Vector2& v, const Matrix4& m, Vector4& result) noexcept;
         static void Transform(_In_reads_(count) const Vector2* varray, size_t count, const Matrix4& m, _Out_writes_(count) Vector4* resultArray) noexcept;

         static void TransformNormal(const Vector2& v, const Matrix4& m, Vector2& result) noexcept;
         static Vector2 TransformNormal(const Vector2& v, const Matrix4& m) noexcept;
         static void TransformNormal(_In_reads_(count) const Vector2* varray, size_t count, const Matrix4& m, _Out_writes_(count) Vector2* resultArray) noexcept;

         // Constants
         static const Vector2 Zero;
         static const Vector2 One;
         static const Vector2 UnitX;
         static const Vector2 UnitY;
     };

     // Binary operators
     Vector2 operator+ (const Vector2& V1, const Vector2& V2) noexcept;
     Vector2 operator- (const Vector2& V1, const Vector2& V2) noexcept;
     Vector2 operator* (const Vector2& V1, const Vector2& V2) noexcept;
     Vector2 operator* (const Vector2& V, float S) noexcept;
     Vector2 operator/ (const Vector2& V1, const Vector2& V2) noexcept;
     Vector2 operator/ (const Vector2& V, float S) noexcept;
     Vector2 operator* (float S, const Vector2& V) noexcept;

     //------------------------------------------------------------------------------
     // 3D vector
     struct Vector3 : public XMFLOAT3
     {
         Vector3() noexcept : XMFLOAT3(0.f, 0.f, 0.f) {}
         constexpr explicit Vector3(float ix) noexcept : XMFLOAT3(ix, ix, ix) {}
         constexpr Vector3(float ix, float iy, float iz) noexcept : XMFLOAT3(ix, iy, iz) {}
         explicit Vector3(_In_reads_(3) const float *pArray) noexcept : XMFLOAT3(pArray) {}
         Vector3(FXMVECTOR V) noexcept { XMStoreFloat3(this, V); }
         Vector3(const XMFLOAT3& V) noexcept { this->x = V.x; this->y = V.y; this->z = V.z; }
         explicit Vector3(const XMVECTORF32& F) noexcept { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; }

         Vector3(const Vector3&) = default;
         Vector3& operator=(const Vector3&) = default;

         Vector3(Vector3&&) = default;
         Vector3& operator=(Vector3&&) = default;

         operator XMVECTOR() const noexcept { return XMLoadFloat3(this); }

         // Comparison operators
         bool operator == (const Vector3& V) const noexcept;
         bool operator != (const Vector3& V) const noexcept;

         // Assignment operators
         Vector3& operator= (const XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; z = F.f[2]; return *this; }
         Vector3& operator+= (const Vector3& V) noexcept;
         Vector3& operator-= (const Vector3& V) noexcept;
         Vector3& operator*= (const Vector3& V) noexcept;
         Vector3& operator*= (float S) noexcept;
         Vector3& operator/= (float S) noexcept;

         // Unary operators
         Vector3 operator+ () const noexcept { return *this; }
         Vector3 operator- () const noexcept;

         // Vector operations
         bool InBounds(const Vector3& Bounds) const noexcept;

         float Length() const noexcept;
         float LengthSquared() const noexcept;

         float Dot(const Vector3& V) const noexcept;
         void Cross(const Vector3& V, Vector3& result) const noexcept;
         Vector3 Cross(const Vector3& V) const noexcept;

         void Normalize() noexcept;
         void Normalize(Vector3& result) const noexcept;

         void Clamp(const Vector3& vmin, const Vector3& vmax) noexcept;
         void Clamp(const Vector3& vmin, const Vector3& vmax, Vector3& result) const noexcept;

         // Static functions
         static float Distance(const Vector3& v1, const Vector3& v2) noexcept;
         static float DistanceSquared(const Vector3& v1, const Vector3& v2) noexcept;

         static void Min(const Vector3& v1, const Vector3& v2, Vector3& result) noexcept;
         static Vector3 Min(const Vector3& v1, const Vector3& v2) noexcept;

         static void Max(const Vector3& v1, const Vector3& v2, Vector3& result) noexcept;
         static Vector3 Max(const Vector3& v1, const Vector3& v2) noexcept;

         static void Lerp(const Vector3& v1, const Vector3& v2, float t, Vector3& result) noexcept;
         static Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) noexcept;

         static void SmoothStep(const Vector3& v1, const Vector3& v2, float t, Vector3& result) noexcept;
         static Vector3 SmoothStep(const Vector3& v1, const Vector3& v2, float t) noexcept;

         static void Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g, Vector3& result) noexcept;
         static Vector3 Barycentric(const Vector3& v1, const Vector3& v2, const Vector3& v3, float f, float g) noexcept;

         static void CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t, Vector3& result) noexcept;
         static Vector3 CatmullRom(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4, float t) noexcept;

         static void Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t, Vector3& result) noexcept;
         static Vector3 Hermite(const Vector3& v1, const Vector3& t1, const Vector3& v2, const Vector3& t2, float t) noexcept;

         static void Reflect(const Vector3& ivec, const Vector3& nvec, Vector3& result) noexcept;
         static Vector3 Reflect(const Vector3& ivec, const Vector3& nvec) noexcept;

         static void Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex, Vector3& result) noexcept;
         static Vector3 Refract(const Vector3& ivec, const Vector3& nvec, float refractionIndex) noexcept;

         static void Transform(const Vector3& v, const Quaternion& quat, Vector3& result) noexcept;
         static Vector3 Transform(const Vector3& v, const Quaternion& quat) noexcept;

         static void Transform(const Vector3& v, const Matrix4& m, Vector3& result) noexcept;
         static Vector3 Transform(const Vector3& v, const Matrix4& m) noexcept;
         static void Transform(_In_reads_(count) const Vector3* varray, size_t count, const Matrix4& m, _Out_writes_(count) Vector3* resultArray) noexcept;

         static void Transform(const Vector3& v, const Matrix4& m, Vector4& result) noexcept;
         static void Transform(_In_reads_(count) const Vector3* varray, size_t count, const Matrix4& m, _Out_writes_(count) Vector4* resultArray) noexcept;

         static void TransformNormal(const Vector3& v, const Matrix4& m, Vector3& result) noexcept;
         static Vector3 TransformNormal(const Vector3& v, const Matrix4& m) noexcept;
         static void TransformNormal(_In_reads_(count) const Vector3* varray, size_t count, const Matrix4& m, _Out_writes_(count) Vector3* resultArray) noexcept;

         // Constants
         static const Vector3 Zero;
         static const Vector3 One;
         static const Vector3 UnitX;
         static const Vector3 UnitY;
         static const Vector3 UnitZ;
         static const Vector3 Up;
         static const Vector3 Down;
         static const Vector3 Right;
         static const Vector3 Left;
         static const Vector3 Forward;
         static const Vector3 Backward;
     };

     // Binary operators
     Vector3 operator+ (const Vector3& V1, const Vector3& V2) noexcept;
     Vector3 operator- (const Vector3& V1, const Vector3& V2) noexcept;
     Vector3 operator* (const Vector3& V1, const Vector3& V2) noexcept;
     Vector3 operator* (const Vector3& V, float S) noexcept;
     Vector3 operator/ (const Vector3& V1, const Vector3& V2) noexcept;
     Vector3 operator/ (const Vector3& V, float S) noexcept;
     Vector3 operator* (float S, const Vector3& V) noexcept;

     //------------------------------------------------------------------------------
     // 4D vector
     struct Vector4 : public XMFLOAT4
     {
         Vector4() noexcept : XMFLOAT4(0.f, 0.f, 0.f, 0.f) {}
         constexpr explicit Vector4(float ix) noexcept : XMFLOAT4(ix, ix, ix, ix) {}
         constexpr Vector4(float ix, float iy, float iz, float iw) noexcept : XMFLOAT4(ix, iy, iz, iw) {}
         constexpr Vector4(Vector3 xyz, float iw) noexcept : XMFLOAT4(xyz.x, xyz.y, xyz.z, iw) {}
         explicit Vector4(_In_reads_(4) const float *pArray) noexcept : XMFLOAT4(pArray) {}
         Vector4(FXMVECTOR V) noexcept { XMStoreFloat4(this, V); }
         Vector4(const XMFLOAT4& V) noexcept { this->x = V.x; this->y = V.y; this->z = V.z; this->w = V.w; }
         explicit Vector4(const XMVECTORF32& F) noexcept { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

         Vector4(const Vector4&) = default;
         Vector4& operator=(const Vector4&) = default;

         Vector4(Vector4&&) = default;
         Vector4& operator=(Vector4&&) = default;

         operator XMVECTOR() const  noexcept { return XMLoadFloat4(this); }

         float GetW() const noexcept { return w; }
         float SetW(float w) noexcept { this->w = w; }

         // Comparison operators
         bool operator == (const Vector4& V) const noexcept;
         bool operator != (const Vector4& V) const noexcept;

         // Assignment operators
         Vector4& operator= (const XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
         Vector4& operator+= (const Vector4& V) noexcept;
         Vector4& operator-= (const Vector4& V) noexcept;
         Vector4& operator*= (const Vector4& V) noexcept;
         Vector4& operator*= (float S) noexcept;
         Vector4& operator/= (float S) noexcept;

         // Unary operators
         Vector4 operator+ () const noexcept { return *this; }
         Vector4 operator- () const noexcept;

         // Vector operations
         bool InBounds(const Vector4& Bounds) const noexcept;

         float Length() const noexcept;
         float LengthSquared() const noexcept;

         float Dot(const Vector4& V) const noexcept;
         void Cross(const Vector4& v1, const Vector4& v2, Vector4& result) const noexcept;
         Vector4 Cross(const Vector4& v1, const Vector4& v2) const noexcept;

         void Normalize() noexcept;
         void Normalize(Vector4& result) const noexcept;

         void Clamp(const Vector4& vmin, const Vector4& vmax) noexcept;
         void Clamp(const Vector4& vmin, const Vector4& vmax, Vector4& result) const noexcept;

         // Static functions
         static float Distance(const Vector4& v1, const Vector4& v2) noexcept;
         static float DistanceSquared(const Vector4& v1, const Vector4& v2) noexcept;

         static void Min(const Vector4& v1, const Vector4& v2, Vector4& result) noexcept;
         static Vector4 Min(const Vector4& v1, const Vector4& v2) noexcept;

         static void Max(const Vector4& v1, const Vector4& v2, Vector4& result) noexcept;
         static Vector4 Max(const Vector4& v1, const Vector4& v2) noexcept;

         static void Lerp(const Vector4& v1, const Vector4& v2, float t, Vector4& result) noexcept;
         static Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t) noexcept;

         static void SmoothStep(const Vector4& v1, const Vector4& v2, float t, Vector4& result) noexcept;
         static Vector4 SmoothStep(const Vector4& v1, const Vector4& v2, float t) noexcept;

         static void Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g, Vector4& result) noexcept;
         static Vector4 Barycentric(const Vector4& v1, const Vector4& v2, const Vector4& v3, float f, float g) noexcept;
          
         static void CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t, Vector4& result) noexcept;
         static Vector4 CatmullRom(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4, float t) noexcept;

         static void Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t, Vector4& result) noexcept;
         static Vector4 Hermite(const Vector4& v1, const Vector4& t1, const Vector4& v2, const Vector4& t2, float t) noexcept;

         static void Reflect(const Vector4& ivec, const Vector4& nvec, Vector4& result) noexcept;
         static Vector4 Reflect(const Vector4& ivec, const Vector4& nvec) noexcept;

         static void Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex, Vector4& result) noexcept;
         static Vector4 Refract(const Vector4& ivec, const Vector4& nvec, float refractionIndex) noexcept;

         static void Transform(const Vector2& v, const Quaternion& quat, Vector4& result) noexcept;
         static Vector4 Transform(const Vector2& v, const Quaternion& quat) noexcept;

         static void Transform(const Vector3& v, const Quaternion& quat, Vector4& result) noexcept;
         static Vector4 Transform(const Vector3& v, const Quaternion& quat) noexcept;

         static void Transform(const Vector4& v, const Quaternion& quat, Vector4& result) noexcept;
         static Vector4 Transform(const Vector4& v, const Quaternion& quat) noexcept;

         static void Transform(const Vector4& v, const Matrix4& m, Vector4& result) noexcept;
         static Vector4 Transform(const Vector4& v, const Matrix4& m) noexcept;
         static void Transform(_In_reads_(count) const Vector4* varray, size_t count, const Matrix4& m, _Out_writes_(count) Vector4* resultArray) noexcept;

         // Constants
         static const Vector4 Zero;
         static const Vector4 One;
         static const Vector4 UnitX;
         static const Vector4 UnitY;
         static const Vector4 UnitZ;
         static const Vector4 UnitW;
     };

     // Binary operators
     Vector4 operator+ (const Vector4& V1, const Vector4& V2) noexcept;
     Vector4 operator- (const Vector4& V1, const Vector4& V2) noexcept;
     Vector4 operator* (const Vector4& V1, const Vector4& V2) noexcept;
     Vector4 operator* (const Vector4& V, float S) noexcept;
     Vector4 operator/ (const Vector4& V1, const Vector4& V2) noexcept;
     Vector4 operator/ (const Vector4& V, float S) noexcept;
     Vector4 operator* (float S, const Vector4& V) noexcept;


     // Represents a 3x3 matrix while occuping a 4x4 memory footprint.  The unused row and column are undefined but implicitly
// (0, 0, 0, 1).  Constructing a Matrix4 will make those values explicit.
     __declspec(align(16)) class Matrix3
     {
     public:
        INLINE Matrix3() {}
        INLINE Matrix3(Vector3 x, Vector3 y, Vector3 z) { m_mat[0] = x; m_mat[1] = y; m_mat[2] = z; }
        INLINE Matrix3(const Matrix3& m) { m_mat[0] = m.m_mat[0]; m_mat[1] = m.m_mat[1]; m_mat[2] = m.m_mat[2]; }
        INLINE Matrix3(const Matrix4& m) noexcept;
        INLINE Matrix3(const Quaternion& q) noexcept;
        INLINE explicit Matrix3(const XMMATRIX& m) { m_mat[0] = Vector3(m.r[0]); m_mat[1] = Vector3(m.r[1]); m_mat[2] = Vector3(m.r[2]); }
        INLINE static Matrix3 Identity() {  return Matrix3(Vector3::UnitX, Vector3::UnitX, Vector3::UnitZ); }
        INLINE static Matrix3 Zero() {  return Matrix3(Vector3::Zero, Vector3::Zero, Vector3::Zero); }

        INLINE void SetX(Vector3 x) { m_mat[0] = x; }
        INLINE void SetY(Vector3 y) { m_mat[1] = y; }
        INLINE void SetZ(Vector3 z) { m_mat[2] = z; }

        INLINE Vector3 GetX() const { return m_mat[0]; }
        INLINE Vector3 GetY() const { return m_mat[1]; }
        INLINE Vector3 GetZ() const { return m_mat[2]; }

        static INLINE Matrix3 MakeXRotation(float angle) { return Matrix3(XMMatrixRotationX(angle)); }
        static INLINE Matrix3 MakeYRotation(float angle) { return Matrix3(XMMatrixRotationY(angle)); }
        static INLINE Matrix3 MakeZRotation(float angle) { return Matrix3(XMMatrixRotationZ(angle)); }
        static INLINE Matrix3 MakeScale(float scale) { return Matrix3(XMMatrixScaling(scale, scale, scale)); }
        static INLINE Matrix3 MakeScale(float sx, float sy, float sz) { return Matrix3(XMMatrixScaling(sx, sy, sz)); }
        static INLINE Matrix3 MakeScale(Vector3 scale) { return Matrix3(XMMatrixScalingFromVector(scale)); }

        // INLINE operator XMMATRIX() const { return (const XMMATRIX&)m_mat; }
        INLINE operator XMMATRIX() const noexcept;

     private:
        Vector3 m_mat[3];
     };

     inline Vector3 operator* (const Vector3& V, const Matrix3& M) noexcept
     {
        return Vector3(
           V.x * M.GetX().x + V.y * M.GetY().x + V.z * M.GetZ().x,
           V.x * M.GetX().y + V.y * M.GetY().y + V.z * M.GetZ().y,
           V.x * M.GetX().z + V.y * M.GetY().z + V.z * M.GetZ().z
           );
     }

     inline Matrix3 operator* (const Matrix3& M1, const Matrix3& M2) noexcept
     {
        return Matrix3(
           M1.GetX() * M2,
           M1.GetY() * M2,
           M1.GetZ() * M2
        );
     }

     //------------------------------------------------------------------------------
     // 4x4 Matrix (assumes right-handed cooordinates)
     struct Matrix4 : public XMFLOAT4X4
     {
         Matrix4() noexcept
             : XMFLOAT4X4(1.f, 0, 0, 0,
                         0, 1.f, 0, 0,
                         0, 0, 1.f, 0,
                         0, 0, 0, 1.f) {}
         constexpr Matrix4(float m00, float m01, float m02, float m03,
                          float m10, float m11, float m12, float m13,
                          float m20, float m21, float m22, float m23,
                          float m30, float m31, float m32, float m33) noexcept
             : XMFLOAT4X4(m00, m01, m02, m03,
                          m10, m11, m12, m13,
                          m20, m21, m22, m23,
                          m30, m31, m32, m33) {}
         explicit Matrix4(const Vector3& r0, const Vector3& r1, const Vector3& r2) noexcept
             : XMFLOAT4X4(r0.x, r0.y, r0.z, 0,
                          r1.x, r1.y, r1.z, 0,
                          r2.x, r2.y, r2.z, 0,
                          0, 0, 0, 1.f) {}
         explicit Matrix4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3) noexcept
             : XMFLOAT4X4(r0.x, r0.y, r0.z, r0.w,
                          r1.x, r1.y, r1.z, r1.w,
                          r2.x, r2.y, r2.z, r2.w,
                          r3.x, r3.y, r3.z, r3.w) {}
         explicit Matrix4(const Matrix3& xyz, Vector3 w) noexcept
             : Matrix4(SetWToZero(xyz.GetX()),
                       SetWToZero(xyz.GetY()),
                       SetWToZero(xyz.GetZ()),
                       SetWToOne(w)) {}
         Matrix4(const XMFLOAT4X4& M) noexcept { memcpy_s(this, sizeof(float) * 16, &M, sizeof(XMFLOAT4X4)); }
         Matrix4(const XMFLOAT3X3& M) noexcept;
         Matrix4(const XMFLOAT4X3& M) noexcept;

         explicit Matrix4(_In_reads_(16) const float *pArray) noexcept : XMFLOAT4X4(pArray) {}
         Matrix4(CXMMATRIX M) noexcept { XMStoreFloat4x4(this, M); }

         Matrix4(const Matrix4&) = default;
         Matrix4& operator=(const Matrix4&) = default;

         Matrix4(Matrix4&&) = default;
         Matrix4& operator=(Matrix4&&) = default;

         operator XMMATRIX() const noexcept { return XMLoadFloat4x4(this); }

         Matrix3 Get3x3() const { return *this; }

         // Comparison operators
         bool operator == (const Matrix4& M) const noexcept;
         bool operator != (const Matrix4& M) const noexcept;

         // Assignment operators
         Matrix4& operator= (const XMFLOAT3X3& M) noexcept;
         Matrix4& operator= (const XMFLOAT4X3& M) noexcept;
         Matrix4& operator+= (const Matrix4& M) noexcept;
         Matrix4& operator-= (const Matrix4& M) noexcept;
         Matrix4& operator*= (const Matrix4& M) noexcept;
         Matrix4& operator*= (float S) noexcept;
         Matrix4& operator/= (float S) noexcept;

         Matrix4& operator/= (const Matrix4& M) noexcept;
             // Element-wise divide

         // Unary operators
         Matrix4 operator+ () const noexcept { return *this; }
         Matrix4 operator- () const noexcept;

         // Properties
         Vector3 Up() const noexcept { return Vector3(_21, _22, _23); }
         void Up(const Vector3& v) noexcept { _21 = v.x; _22 = v.y; _23 = v.z; }

         Vector3 Down() const  noexcept { return Vector3(-_21, -_22, -_23); }
         void Down(const Vector3& v) noexcept { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

         Vector3 Right() const noexcept { return Vector3(_11, _12, _13); }
         void Right(const Vector3& v) noexcept { _11 = v.x; _12 = v.y; _13 = v.z; }

         Vector3 Left() const noexcept { return Vector3(-_11, -_12, -_13); }
         void Left(const Vector3& v) noexcept { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

         Vector3 Forward() const noexcept { return Vector3(-_31, -_32, -_33); }
         void Forward(const Vector3& v) noexcept { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

         Vector3 Backward() const noexcept { return Vector3(_31, _32, _33); }
         void Backward(const Vector3& v) noexcept { _31 = v.x; _32 = v.y; _33 = v.z; }

         Vector3 Translation() const  noexcept { return Vector3(_41, _42, _43); }
         void Translation(const Vector3& v) noexcept { _41 = v.x; _42 = v.y; _43 = v.z; }

         // Matrix operations
         bool Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) noexcept;

         Matrix4 Transpose() const noexcept;
         void Transpose(Matrix4& result) const noexcept;

         Matrix4 Invert() const noexcept;
         void Invert(Matrix4& result) const noexcept;

         float Determinant() const noexcept;

         // Static functions
         static Matrix4 CreateBillboard(
             const Vector3& object, const Vector3& cameraPosition, const Vector3& cameraUp, _In_opt_ const Vector3* cameraForward = nullptr) noexcept;

         static Matrix4 CreateConstrainedBillboard(
             const Vector3& object, const Vector3& cameraPosition, const Vector3& rotateAxis,
             _In_opt_ const Vector3* cameraForward = nullptr, _In_opt_ const Vector3* objectForward = nullptr) noexcept;

         static Matrix4 CreateTranslation(const Vector3& position) noexcept;
         static Matrix4 CreateTranslation(float x, float y, float z) noexcept;

         static Matrix4 CreateScale(const Vector3& scales) noexcept;
         static Matrix4 CreateScale(float xs, float ys, float zs) noexcept;
         static Matrix4 CreateScale(float scale) noexcept;

         static Matrix4 CreateRotationX(float radians) noexcept;
         static Matrix4 CreateRotationY(float radians) noexcept;
         static Matrix4 CreateRotationZ(float radians) noexcept;

         static Matrix4 CreateFromAxisAngle(const Vector3& axis, float angle) noexcept;

         static Matrix4 CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane) noexcept;
         static Matrix4 CreatePerspective(float width, float height, float nearPlane, float farPlane) noexcept;
         static Matrix4 CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlane, float farPlane) noexcept;
         static Matrix4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane) noexcept;
         static Matrix4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane) noexcept;

         static Matrix4 CreateLookAt(const Vector3& position, const Vector3& target, const Vector3& up) noexcept;
         static Matrix4 CreateWorld(const Vector3& position, const Vector3& forward, const Vector3& up) noexcept;

         static Matrix4 CreateFromQuaternion(const Quaternion& quat) noexcept;

         static Matrix4 CreateFromYawPitchRoll(float yaw, float pitch, float roll) noexcept;

         static Matrix4 CreateShadow(const Vector3& lightDir, const Plane& plane) noexcept;

         static Matrix4 CreateReflection(const Plane& plane) noexcept;

         static void Lerp(const Matrix4& M1, const Matrix4& M2, float t, Matrix4& result) noexcept;
         static Matrix4 Lerp(const Matrix4& M1, const Matrix4& M2, float t) noexcept;

         static void Transform(const Matrix4& M, const Quaternion& rotation, Matrix4& result) noexcept;
         static Matrix4 Transform(const Matrix4& M, const Quaternion& rotation) noexcept;

         // Constants
         static const Matrix4 Identity;
     };

     // Binary operators
     Matrix4 operator+ (const Matrix4& M1, const Matrix4& M2) noexcept;
     Matrix4 operator- (const Matrix4& M1, const Matrix4& M2) noexcept;
     Matrix4 operator* (const Matrix4& M1, const Matrix4& M2) noexcept;
     Vector3 operator* (const Vector3& V, const Matrix4& M1) noexcept;
     Vector4 operator* (const Vector4& V, const Matrix4& M1) noexcept;
     Matrix4 operator* (const Matrix4& M, float S) noexcept;
     Matrix4 operator/ (const Matrix4& M, float S) noexcept;
     Matrix4 operator/ (const Matrix4& M1, const Matrix4& M2) noexcept;
         // Element-wise divide
     Matrix4 operator* (float S, const Matrix4& M) noexcept;


     //-----------------------------------------------------------------------------
     // Plane
     struct Plane : public XMFLOAT4
     {
         Plane() noexcept : XMFLOAT4(0.f, 1.f, 0.f, 0.f) {}
         constexpr Plane(float ix, float iy, float iz, float iw) noexcept : XMFLOAT4(ix, iy, iz, iw) {}
         Plane(const Vector3& normal, float d) noexcept : XMFLOAT4(normal.x, normal.y, normal.z, d) {}
         Plane(const Vector3& point1, const Vector3& point2, const Vector3& point3) noexcept;
         Plane(const Vector3& point, const Vector3& normal) noexcept;
         explicit Plane(const Vector4& v) noexcept : XMFLOAT4(v.x, v.y, v.z, v.w) {}
         explicit Plane(_In_reads_(4) const float *pArray) noexcept : XMFLOAT4(pArray) {}
         Plane(FXMVECTOR V) noexcept { XMStoreFloat4(this, V); }
         Plane(const XMFLOAT4& p) noexcept { this->x = p.x; this->y = p.y; this->z = p.z; this->w = p.w; }
         explicit Plane(const XMVECTORF32& F) noexcept { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

         Plane(const Plane&) = default;
         Plane& operator=(const Plane&) = default;

         Plane(Plane&&) = default;
         Plane& operator=(Plane&&) = default;

         operator XMVECTOR() const noexcept { return XMLoadFloat4(this); }

         // Comparison operators
         bool operator == (const Plane& p) const noexcept;
         bool operator != (const Plane& p) const noexcept;

         // Assignment operators
         Plane& operator= (const XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }

         // Properties
         Vector3 Normal() const noexcept { return Vector3(x, y, z); }
         void Normal(const Vector3& normal) noexcept { x = normal.x; y = normal.y; z = normal.z; }

         float D() const noexcept { return w; }
         void D(float d) noexcept { w = d; }

         // Plane operations
         void Normalize() noexcept;
         void Normalize(Plane& result) const noexcept;

         float Dot(const Vector4& v) const noexcept;
         float DotCoordinate(const Vector3& position) const noexcept;
         float DotNormal(const Vector3& normal) const noexcept;

         // Static functions
         static void Transform(const Plane& plane, const Matrix4& M, Plane& result) noexcept;
         static Plane Transform(const Plane& plane, const Matrix4& M) noexcept;

         static void Transform(const Plane& plane, const Quaternion& rotation, Plane& result) noexcept;
         static Plane Transform(const Plane& plane, const Quaternion& rotation) noexcept;
             // Input quaternion must be the inverse transpose of the transformation
     };

     //------------------------------------------------------------------------------
     // Quaternion
     struct Quaternion : public XMFLOAT4
     {
         Quaternion() noexcept : XMFLOAT4(0, 0, 0, 1.f) {}
         constexpr Quaternion(float ix, float iy, float iz, float iw) noexcept : XMFLOAT4(ix, iy, iz, iw) {}
         Quaternion(const Vector3& v, float scalar) noexcept : XMFLOAT4(v.x, v.y, v.z, scalar) {}
         explicit Quaternion(const Vector4& v) noexcept : XMFLOAT4(v.x, v.y, v.z, v.w) {}
         explicit Quaternion(_In_reads_(4) const float *pArray) noexcept : XMFLOAT4(pArray) {}
         Quaternion(FXMVECTOR V) noexcept { XMStoreFloat4(this, V); }
         Quaternion(const XMFLOAT4& q) noexcept { this->x = q.x; this->y = q.y; this->z = q.z; this->w = q.w; }
         explicit Quaternion(const XMVECTORF32& F) noexcept { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }
         explicit Quaternion(const FXMMATRIX& M) noexcept { *this = XMQuaternionRotationMatrix(M); }
         explicit Quaternion(const Matrix3& M) noexcept
         {
            // todo:
            Matrix4 MM{M.GetX(), M.GetY(), M.GetZ()};
            *this = XMQuaternionRotationMatrix(MM);
         }

         Quaternion(const Quaternion&) = default;
         Quaternion& operator=(const Quaternion&) = default;

         Quaternion(Quaternion&&) = default;
         Quaternion& operator=(Quaternion&&) = default;

         operator XMVECTOR() const noexcept { return XMLoadFloat4(this); }

         Quaternion operator~ () const noexcept;

         // Comparison operators
         bool operator == (const Quaternion& q) const noexcept;
         bool operator != (const Quaternion& q) const noexcept;

         // Assignment operators
         Quaternion& operator= (const XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
         Quaternion& operator+= (const Quaternion& q) noexcept;
         Quaternion& operator-= (const Quaternion& q) noexcept;
         Quaternion& operator*= (const Quaternion& q) noexcept;
         Quaternion& operator*= (float S) noexcept;
         Quaternion& operator/= (const Quaternion& q) noexcept;

         // Unary operators
         Quaternion operator+ () const  noexcept { return *this; }
         Quaternion operator- () const noexcept;

         // Quaternion operations
         float Length() const noexcept;
         float LengthSquared() const noexcept;

         void Normalize() noexcept;
         void Normalize(Quaternion& result) const noexcept;

         void Conjugate() noexcept;
         void Conjugate(Quaternion& result) const noexcept;

         void Inverse(Quaternion& result) const noexcept;

         float Dot(const Quaternion& Q) const noexcept;

         // Static functions
         static Quaternion CreateFromAxisAngle(const Vector3& axis, float angle) noexcept;
         static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll) noexcept;
         static Quaternion CreateFromRotationMatrix(const Matrix4& M) noexcept;

         static void Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result) noexcept;
         static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept;

         static void Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result) noexcept;
         static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept;

         static void Concatenate(const Quaternion& q1, const Quaternion& q2, Quaternion& result) noexcept;
         static Quaternion Concatenate(const Quaternion& q1, const Quaternion& q2) noexcept;

         // Constants
         static const Quaternion Identity;
     };

     // Binary operators
     Quaternion operator+ (const Quaternion& Q1, const Quaternion& Q2) noexcept;
     Quaternion operator- (const Quaternion& Q1, const Quaternion& Q2) noexcept;
     Quaternion operator* (const Quaternion& Q1, const Quaternion& Q2) noexcept;
     Quaternion operator* (const Quaternion& Q, float S) noexcept;
     Vector3    operator* (const Vector3& v, const Quaternion& Q) noexcept;
     Quaternion operator/ (const Quaternion& Q1, const Quaternion& Q2) noexcept;
     Quaternion operator* (float S, const Quaternion& Q) noexcept;

     //------------------------------------------------------------------------------
     // Color
     struct Color : public XMFLOAT4
     {
         Color() noexcept : XMFLOAT4(0, 0, 0, 1.f) {}
         constexpr Color(float _r, float _g, float _b) noexcept : XMFLOAT4(_r, _g, _b, 1.f) {}
         constexpr Color(float _r, float _g, float _b, float _a) noexcept : XMFLOAT4(_r, _g, _b, _a) {}
         explicit Color(const Vector3& clr) noexcept : XMFLOAT4(clr.x, clr.y, clr.z, 1.f) {}
         explicit Color(const Vector4& clr) noexcept : XMFLOAT4(clr.x, clr.y, clr.z, clr.w) {}
         explicit Color(_In_reads_(4) const float *pArray) noexcept : XMFLOAT4(pArray) {}
         Color(FXMVECTOR V) noexcept { XMStoreFloat4(this, V); }
         Color(const XMFLOAT4& c) noexcept { this->x = c.x; this->y = c.y; this->z = c.z; this->w = c.w; }
         explicit Color(const XMVECTORF32& F) noexcept { this->x = F.f[0]; this->y = F.f[1]; this->z = F.f[2]; this->w = F.f[3]; }

         explicit Color(const DirectX::PackedVector::XMCOLOR& Packed) noexcept;
             // BGRA Direct3D 9 D3DCOLOR packed color

         explicit Color(const DirectX::PackedVector::XMUBYTEN4& Packed) noexcept;
             // RGBA XNA Game Studio packed color

         Color(const Color&) = default;
         Color& operator=(const Color&) = default;

         Color(Color&&) = default;
         Color& operator=(Color&&) = default;

         operator XMVECTOR() const noexcept { return XMLoadFloat4(this); }
         operator const float*() const noexcept { return reinterpret_cast<const float*>(this); }

         const float* GetPtr() const noexcept { return reinterpret_cast<const float*>(this); }
         float* GetPtr() noexcept { return reinterpret_cast<float*>(this); }

         // Comparison operators
         bool operator == (const Color& c) const noexcept;
         bool operator != (const Color& c) const noexcept;

         // Assignment operators
         Color& operator= (const XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
         Color& operator= (const DirectX::PackedVector::XMCOLOR& Packed) noexcept;
         Color& operator= (const DirectX::PackedVector::XMUBYTEN4& Packed) noexcept;
         Color& operator+= (const Color& c) noexcept;
         Color& operator-= (const Color& c) noexcept;
         Color& operator*= (const Color& c) noexcept;
         Color& operator*= (float S) noexcept;
         Color& operator/= (const Color& c) noexcept;

         // Unary operators
         Color operator+ () const noexcept { return *this; }
         Color operator- () const noexcept;

         // Properties
         float R() const noexcept { return x; }
         void R(float r) noexcept { x = r; }

         float G() const noexcept { return y; }
         void G(float g) noexcept { y = g; }

         float B() const noexcept { return z; }
         void B(float b) noexcept { z = b; }

         float A() const noexcept { return w; }
         void A(float a) noexcept { w = a; }

         // Color operations
         DirectX::PackedVector::XMCOLOR BGRA() const noexcept;
         DirectX::PackedVector::XMUBYTEN4 RGBA() const noexcept;

         Vector3 ToVector3() const noexcept;
         Vector4 ToVector4() const noexcept;

         void Negate() noexcept;
         void Negate(Color& result) const noexcept;

         void Saturate() noexcept;
         void Saturate(Color& result) const noexcept;

         void Premultiply() noexcept;
         void Premultiply(Color& result) const noexcept;

         void AdjustSaturation(float sat) noexcept;
         void AdjustSaturation(float sat, Color& result) const noexcept;

         void AdjustContrast(float contrast) noexcept;
         void AdjustContrast(float contrast, Color& result) const noexcept;

         // Static functions
         static void Modulate(const Color& c1, const Color& c2, Color& result) noexcept;
         static Color Modulate(const Color& c1, const Color& c2) noexcept;

         static void Lerp(const Color& c1, const Color& c2, float t, Color& result) noexcept;
         static Color Lerp(const Color& c1, const Color& c2, float t) noexcept;
     };

     // Binary operators
     Color operator+ (const Color& C1, const Color& C2) noexcept;
     Color operator- (const Color& C1, const Color& C2) noexcept;
     Color operator* (const Color& C1, const Color& C2) noexcept;
     Color operator* (const Color& C, float S) noexcept;
     Color operator/ (const Color& C1, const Color& C2) noexcept;
     Color operator* (float S, const Color& C) noexcept;

     //------------------------------------------------------------------------------
     // Ray
     class Ray
     {
     public:
         Vector3 position;
         Vector3 direction;

         Ray() noexcept : position(0, 0, 0), direction(0, 0, 1) {}
         Ray(const Vector3& pos, const Vector3& dir) noexcept : position(pos), direction(dir) {}

         Ray(const Ray&) = default;
         Ray& operator=(const Ray&) = default;

         Ray(Ray&&) = default;
         Ray& operator=(Ray&&) = default;

         // Comparison operators
         bool operator == (const Ray& r) const noexcept;
         bool operator != (const Ray& r) const noexcept;

         // Ray operations
         bool Intersects(const BoundingSphere& sphere, _Out_ float& Dist) const noexcept;
         bool Intersects(const BoundingBox& box, _Out_ float& Dist) const noexcept;
         bool Intersects(const Vector3& tri0, const Vector3& tri1, const Vector3& tri2, _Out_ float& Dist) const noexcept;
         bool Intersects(const Plane& plane, _Out_ float& Dist) const noexcept;
     };

     //------------------------------------------------------------------------------
     // Viewport
     class Viewport
     {
     public:
         float x;
         float y;
         float width;
         float height;
         float minDepth;
         float maxDepth;

         Viewport() noexcept :
             x(0.f), y(0.f), width(0.f), height(0.f), minDepth(0.f), maxDepth(1.f) {}
         constexpr Viewport(float ix, float iy, float iw, float ih, float iminz = 0.f, float imaxz = 1.f) noexcept :
             x(ix), y(iy), width(iw), height(ih), minDepth(iminz), maxDepth(imaxz) {}
         explicit Viewport(const RECT& rct) noexcept :
             x(float(rct.left)), y(float(rct.top)),
             width(float(rct.right - rct.left)),
             height(float(rct.bottom - rct.top)),
             minDepth(0.f), maxDepth(1.f) {}

     #if defined(__d3d11_h__) || defined(__d3d11_x_h__)
         // Direct3D 11 interop
         explicit Viewport(const D3D11_VIEWPORT& vp) noexcept :
             x(vp.TopLeftX), y(vp.TopLeftY),
             width(vp.Width), height(vp.Height),
             minDepth(vp.MinDepth), maxDepth(vp.MaxDepth) {}

         operator D3D11_VIEWPORT() noexcept { return *reinterpret_cast<const D3D11_VIEWPORT*>(this); }
         const D3D11_VIEWPORT* Get11() const noexcept { return reinterpret_cast<const D3D11_VIEWPORT*>(this); }
         Viewport& operator= (const D3D11_VIEWPORT& vp) noexcept;
     #endif

     #if defined(__d3d12_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
         // Direct3D 12 interop
         explicit Viewport(const D3D12_VIEWPORT& vp) noexcept :
             x(vp.TopLeftX), y(vp.TopLeftY),
             width(vp.Width), height(vp.Height),
             minDepth(vp.MinDepth), maxDepth(vp.MaxDepth) {}

         operator D3D12_VIEWPORT() noexcept { return *reinterpret_cast<const D3D12_VIEWPORT*>(this); }
         const D3D12_VIEWPORT* Get12() const noexcept { return reinterpret_cast<const D3D12_VIEWPORT*>(this); }
         Viewport& operator= (const D3D12_VIEWPORT& vp) noexcept;
     #endif

         Viewport(const Viewport&) = default;
         Viewport& operator=(const Viewport&) = default;

         Viewport(Viewport&&) = default;
         Viewport& operator=(Viewport&&) = default;

         // Comparison operators
         bool operator == (const Viewport& vp) const noexcept;
         bool operator != (const Viewport& vp) const noexcept;

         // Assignment operators
         Viewport& operator= (const RECT& rct) noexcept;

         // Viewport operations
         float AspectRatio() const noexcept;

         Vector3 Project(const Vector3& p, const Matrix4& proj, const Matrix4& view, const Matrix4& world) const noexcept;
         void Project(const Vector3& p, const Matrix4& proj, const Matrix4& view, const Matrix4& world, Vector3& result) const noexcept;

         Vector3 Unproject(const Vector3& p, const Matrix4& proj, const Matrix4& view, const Matrix4& world) const noexcept;
         void Unproject(const Vector3& p, const Matrix4& proj, const Matrix4& view, const Matrix4& world, Vector3& result) const noexcept;

         // Static methods
         static RECT __cdecl ComputeDisplayArea(DXGI_SCALING scaling, UINT backBufferWidth, UINT backBufferHeight, int outputWidth, int outputHeight) noexcept;
         static RECT __cdecl ComputeTitleSafeArea(UINT backBufferWidth, UINT backBufferHeight) noexcept;
     };

 #include "Math.inl"

 } // namespace Math

//------------------------------------------------------------------------------
// Support for Math and Standard C++ Library containers
namespace std
{

    template<> struct less<Math::Rectangle>
    {
        bool operator()(const Math::Rectangle& r1, const Math::Rectangle& r2) const noexcept
        {
            return ((r1.x < r2.x)
                    || ((r1.x == r2.x) && (r1.y < r2.y))
                    || ((r1.x == r2.x) && (r1.y == r2.y) && (r1.width < r2.width))
                    || ((r1.x == r2.x) && (r1.y == r2.y) && (r1.width == r2.width) && (r1.height < r2.height)));
        }
    };

    template<> struct less<Math::Vector2>
    {
        bool operator()(const Math::Vector2& V1, const Math::Vector2& V2) const noexcept
        {
            return ((V1.x < V2.x) || ((V1.x == V2.x) && (V1.y < V2.y)));
        }
    };

    template<> struct less<Math::Vector3>
    {
        bool operator()(const Math::Vector3& V1, const Math::Vector3& V2) const noexcept
        {
            return ((V1.x < V2.x)
                    || ((V1.x == V2.x) && (V1.y < V2.y))
                    || ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z < V2.z)));
        }
    };

    template<> struct less<Math::Vector4>
    {
        bool operator()(const Math::Vector4& V1, const Math::Vector4& V2) const noexcept
        {
            return ((V1.x < V2.x)
                    || ((V1.x == V2.x) && (V1.y < V2.y))
                    || ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z < V2.z))
                    || ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z == V2.z) && (V1.w < V2.w)));
        }
    };

    template<> struct less<Math::Matrix4>
    {
        bool operator()(const Math::Matrix4& M1, const Math::Matrix4& M2) const noexcept
        {
            if (M1._11 != M2._11) return M1._11 < M2._11;
            if (M1._12 != M2._12) return M1._12 < M2._12;
            if (M1._13 != M2._13) return M1._13 < M2._13;
            if (M1._14 != M2._14) return M1._14 < M2._14;
            if (M1._21 != M2._21) return M1._21 < M2._21;
            if (M1._22 != M2._22) return M1._22 < M2._22;
            if (M1._23 != M2._23) return M1._23 < M2._23;
            if (M1._24 != M2._24) return M1._24 < M2._24;
            if (M1._31 != M2._31) return M1._31 < M2._31;
            if (M1._32 != M2._32) return M1._32 < M2._32;
            if (M1._33 != M2._33) return M1._33 < M2._33;
            if (M1._34 != M2._34) return M1._34 < M2._34;
            if (M1._41 != M2._41) return M1._41 < M2._41;
            if (M1._42 != M2._42) return M1._42 < M2._42;
            if (M1._43 != M2._43) return M1._43 < M2._43;
            if (M1._44 != M2._44) return M1._44 < M2._44;

            return false;
        }
    };

    template<> struct less<Math::Plane>
    {
        bool operator()(const Math::Plane& P1, const Math::Plane& P2) const noexcept
        {
            return ((P1.x < P2.x)
                    || ((P1.x == P2.x) && (P1.y < P2.y))
                    || ((P1.x == P2.x) && (P1.y == P2.y) && (P1.z < P2.z))
                    || ((P1.x == P2.x) && (P1.y == P2.y) && (P1.z == P2.z) && (P1.w < P2.w)));
        }
    };

    template<> struct less<Math::Quaternion>
    {
        bool operator()(const Math::Quaternion& Q1, const Math::Quaternion& Q2) const noexcept
        {
            return ((Q1.x < Q2.x)
                    || ((Q1.x == Q2.x) && (Q1.y < Q2.y))
                    || ((Q1.x == Q2.x) && (Q1.y == Q2.y) && (Q1.z < Q2.z))
                    || ((Q1.x == Q2.x) && (Q1.y == Q2.y) && (Q1.z == Q2.z) && (Q1.w < Q2.w)));
        }
    };

    template<> struct less<Math::Color>
    {
        bool operator()(const Math::Color& C1, const Math::Color& C2) const noexcept
        {
            return ((C1.x < C2.x)
                    || ((C1.x == C2.x) && (C1.y < C2.y))
                    || ((C1.x == C2.x) && (C1.y == C2.y) && (C1.z < C2.z))
                    || ((C1.x == C2.x) && (C1.y == C2.y) && (C1.z == C2.z) && (C1.w < C2.w)));
        }
    };

    template<> struct less<Math::Ray>
    {
        bool operator()(const Math::Ray& R1, const Math::Ray& R2) const noexcept
        {
            if (R1.position.x != R2.position.x) return R1.position.x < R2.position.x;
            if (R1.position.y != R2.position.y) return R1.position.y < R2.position.y;
            if (R1.position.z != R2.position.z) return R1.position.z < R2.position.z;

            if (R1.direction.x != R2.direction.x) return R1.direction.x < R2.direction.x;
            if (R1.direction.y != R2.direction.y) return R1.direction.y < R2.direction.y;
            if (R1.direction.z != R2.direction.z) return R1.direction.z < R2.direction.z;

            return false;
        }
    };

    template<> struct less<Math::Viewport>
    {
        bool operator()(const Math::Viewport& vp1, const Math::Viewport& vp2) const noexcept
        {
            if (vp1.x != vp2.x) return (vp1.x < vp2.x);
            if (vp1.y != vp2.y) return (vp1.y < vp2.y);

            if (vp1.width != vp2.width) return (vp1.width < vp2.width);
            if (vp1.height != vp2.height) return (vp1.height < vp2.height);

            if (vp1.minDepth != vp2.minDepth) return (vp1.minDepth < vp2.minDepth);
            if (vp1.maxDepth != vp2.maxDepth) return (vp1.maxDepth < vp2.maxDepth);

            return false;
        }
    };

} // namespace std

#ifdef __clang__
#pragma clang diagnostic pop
#endif
