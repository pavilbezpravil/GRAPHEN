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

#pragma once

#include "gnpch.h"
#include "Graphen/Math/Frustum.h"

namespace Math
{
    class BaseCamera {
    public:

        // Call this function once per frame and after you've changed any state.  This
        // regenerates all matrices.  Calling it more or less than once per frame will break
        // temporal effects and cause unpredictable results.
        void Update();

        // Public functions for controlling where the camera is and its orientation
        void SetEyeAtUp( Vector3 eye, Vector3 at, Vector3 up );
        void SetLookDirection( Vector3 forward, Vector3 up );
        void SetRotation( Quaternion basisRotation );
        void SetPosition( Vector3 worldPos );
        void SetTransform( const AffineTransform& xform );
        void SetTransform( const OrthogonalTransform& xform );

        const Quaternion GetRotation() const { return m_CameraToWorld.GetRotation(); }
        const Vector3 GetRightVec() const { return m_Basis.GetX(); }
        const Vector3 GetUpVec() const { return m_Basis.GetY(); }
        const Vector3 GetForwardVec() const { return -m_Basis.GetZ(); }
        const Vector3 GetPosition() const { return m_CameraToWorld.GetTranslation(); }

        // Accessors for reading the various matrices and frusta
        const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
        const Matrix4& GetProjMatrix() const { return m_ProjMatrix; }
        const Matrix4& GetViewProjMatrix() const { return m_ViewProjMatrix; }
        const Matrix4& GetReprojectionMatrix() const { return m_ReprojectMatrix; }
        const Frustum& GetViewSpaceFrustum() const { return m_FrustumVS; }
        const Frustum& GetWorldSpaceFrustum() const { return m_FrustumWS; }

    protected:

        BaseCamera() : m_CameraToWorld(OrthogonalTransform::Identity()), m_Basis(Matrix3::Identity()) {}

        void SetProjMatrix( const Matrix4& ProjMat ) { m_ProjMatrix = ProjMat; }

        OrthogonalTransform m_CameraToWorld;

        Matrix3 m_Basis;

        Matrix4 m_ViewMatrix;
        Matrix4 m_ProjMatrix;
        Matrix4 m_ViewProjMatrix;

        Matrix4 m_PreviousViewProjMatrix;

        Matrix4 m_ReprojectMatrix;

        Frustum m_FrustumVS;
        Frustum m_FrustumWS;
    };

    class ShadowCamera : public BaseCamera {
    public:

       ShadowCamera() {}

       void UpdateMatrix(
          Vector3 LightDirection,        // Direction parallel to light, in direction of travel
          Vector3 ShadowCenter,        // Center location on far bounding plane of shadowed region
          Vector3 ShadowBounds,        // Width, height, and depth in world space represented by the shadow buffer
          uint32_t BufferWidth,        // Shadow buffer width
          uint32_t BufferHeight,        // Shadow buffer height--usually same as width
          uint32_t BufferPrecision    // Bit depth of shadow buffer--usually 16 or 24
       );

       // Used to transform world space to texture space for shadow sampling
       const Matrix4& GetShadowMatrix() const { return m_ShadowMatrix; }

    private:

       Matrix4 m_ShadowMatrix;
    };

    class Camera : public BaseCamera {
    public:
        Camera();

        // Controls the view-to-projection matrix
        void SetPerspectiveMatrix( float verticalFovRadians, float aspectHeightOverWidth, float nearZClip, float farZClip );
        void SetFOV( float verticalFovInRadians ) { m_VerticalFOV = verticalFovInRadians; UpdateProjMatrix(); }
        void SetAspectRatio( float heightOverWidth ) { m_AspectRatio = heightOverWidth; UpdateProjMatrix(); }
        void SetZRange( float nearZ, float farZ) { m_NearClip = nearZ; m_FarClip = farZ; UpdateProjMatrix(); }
        void ReverseZ( bool enable ) { m_ReverseZ = enable; UpdateProjMatrix(); }

        float GetFOV() const { return m_VerticalFOV; }
        float GetNearClip() const { return m_NearClip; }
        float GetFarClip() const { return m_FarClip; }
        float GetClearDepth() const { return m_ReverseZ ? 0.0f : 1.0f; }

    private:

        void UpdateProjMatrix( void );

        float m_VerticalFOV;            // Field of view angle in radians
        float m_AspectRatio;
        float m_NearClip;
        float m_FarClip;
        bool m_ReverseZ;                // Invert near and far clip distances so that Z=0 is the far plane
    };

    inline void BaseCamera::SetEyeAtUp( Vector3 eye, Vector3 at, Vector3 up )
    {
        SetLookDirection(at - eye, up);
        SetPosition(eye);
    }

    inline void BaseCamera::SetPosition( Vector3 worldPos )
    {
        m_CameraToWorld.SetTranslation( worldPos );
    }

    inline void BaseCamera::SetTransform( const AffineTransform& xform )
    {
        // By using these functions, we rederive an orthogonal transform.
        SetLookDirection(-xform.GetZ(), xform.GetY());
        SetPosition(xform.GetTranslation());
    }

    inline void BaseCamera::SetRotation( Quaternion basisRotation )
    {
        m_CameraToWorld.SetRotation(Normalize(basisRotation));
        m_Basis = Matrix3(m_CameraToWorld.GetRotation());
    }

    inline Camera::Camera() : m_ReverseZ(true)
    {
        SetPerspectiveMatrix( XM_PIDIV4, 9.0f / 16.0f, 0.1f, 100.0f );
    }

    inline void Camera::SetPerspectiveMatrix( float verticalFovRadians, float aspectHeightOverWidth, float nearZClip, float farZClip )
    {
        m_VerticalFOV = verticalFovRadians;
        m_AspectRatio = aspectHeightOverWidth;
        m_NearClip = nearZClip;
        m_FarClip = farZClip;

        UpdateProjMatrix();

        m_PreviousViewProjMatrix = m_ViewProjMatrix;
    }
} // namespace Math
