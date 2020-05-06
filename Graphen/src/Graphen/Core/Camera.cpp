//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard 
//

#include "gnpch.h"
#include "Camera.h"
#include <cmath>

using namespace Math;

void BaseCamera::SetLookDirection( Vector3 forward, Vector3 up )
{
    // Given, but ensure normalization
    Scalar forwardLenSq = LengthSquare(forward);
    forward = Select(forward * RecipSqrt(forwardLenSq), -Vector3::UnitZ, forwardLenSq < Scalar(0.000001f));

    // Deduce a valid, orthogonal right vector
    Vector3 right = Cross(forward, up);
    Scalar rightLenSq = LengthSquare(right);
    right = Select(right * RecipSqrt(rightLenSq), Quaternion(Vector3::UnitY, -XM_PIDIV2) * forward, rightLenSq < Scalar(0.000001f));

    // Compute actual up vector
    up = Cross(right, forward);

    // Finish constructing basis
    m_Basis = Matrix3(right, up, -forward);
    m_CameraToWorld.SetRotation(Quaternion(m_Basis));
}

void BaseCamera::Update()
{
    m_PreviousViewProjMatrix = m_ViewProjMatrix;

    auto viewMatrix = ~m_CameraToWorld;
    m_ViewMatrix = Matrix4(viewMatrix.GetRotation(), viewMatrix.GetTranslation());
    m_ViewProjMatrix = m_ViewMatrix * m_ProjMatrix;
    m_ReprojectMatrix = Invert(GetViewProjMatrix()) * m_PreviousViewProjMatrix;

    m_FrustumVS = Frustum( m_ProjMatrix );
    m_FrustumWS = m_FrustumVS * m_CameraToWorld;
}

void ShadowCamera::UpdateMatrix(Vector3 LightDirection, Vector3 ShadowCenter, Vector3 ShadowBounds,
   uint32_t BufferWidth, uint32_t BufferHeight, uint32_t BufferPrecision) {
   SetLookDirection(LightDirection, Vector3::UnitZ);

   Vector3 RcpDimensions = Recip(ShadowBounds);
   Vector3 QuantizeScale = Vector3((float)BufferWidth, (float)BufferHeight, BufferPrecision == 32 ? (float)UINT_MAX : (float)((1 << BufferPrecision) - 1)) * RcpDimensions;

   // Transform to view space
   ShadowCenter = ShadowCenter * ~GetRotation();
   // Scale to texel units, truncate fractional part, and scale back to world units
   ShadowCenter = Floor(ShadowCenter * QuantizeScale) / QuantizeScale;
   // Transform back into world space
   ShadowCenter = ShadowCenter * GetRotation();

   SetPosition(ShadowCenter);

   SetProjMatrix(Matrix4::CreateScale(RcpDimensions * Vector3(2.0f, 2.0f, 1.0f)));

   Update();

   // Transform from clip space to texture space
   m_ShadowMatrix = m_ViewProjMatrix; // *Matrix4(AffineTransform(Matrix3::MakeScale(0.5f, -0.5f, 1.0f), Vector3(0.5f, 0.5f, 0.0f)));
}

void Camera::UpdateProjMatrix( void )
{
   if (true)
   {
      float Y = 1.0f / std::tanf(m_VerticalFOV * 0.5f);
      float X = Y * m_AspectRatio;

      float Q1, Q2;

      // ReverseZ puts far plane at Z=0 and near plane at Z=1.  This is never a bad idea, and it's
      // actually a great idea with F32 depth buffers to redistribute precision more evenly across
      // the entire range.  It requires clearing Z to 0.0f and using a GREATER variant depth test.
      // Some care must also be done to properly reconstruct linear W in a pixel shader from hyperbolic Z.
      if (m_ReverseZ)
      {
         Q1 = m_NearClip / (m_FarClip - m_NearClip);
         Q2 = Q1 * m_FarClip;
      }
      else
      {
         Q1 = m_FarClip / (m_NearClip - m_FarClip);
         Q2 = Q1 * m_NearClip;
      }

      SetProjMatrix(Matrix4(
         Vector4(X, 0.0f, 0.0f, 0.0f),
         Vector4(0.0f, Y, 0.0f, 0.0f),
         Vector4(0.0f, 0.0f, Q1, -1.0f),
         Vector4(0.0f, 0.0f, Q2, 0.0f)
      ));
   }
   else
   {
      SetProjMatrix(
         XMMatrixPerspectiveFovLH(m_VerticalFOV, m_AspectRatio, m_NearClip, m_FarClip)
      );
   }
}
