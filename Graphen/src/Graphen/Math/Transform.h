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

// #include "Matrix3.h"
#include "Math.h"

namespace Math
{
    // This transform strictly prohibits non-uniform scale.  Scale itself is barely tolerated.
    __declspec(align(16)) class OrthogonalTransform
    {
    public:
        INLINE OrthogonalTransform() : m_rotation(Quaternion::Identity), m_translation(Vector3::Zero) {}
        INLINE OrthogonalTransform( Quaternion rotate ) : m_rotation(rotate), m_translation(Vector3::Zero) {}
        INLINE OrthogonalTransform( Vector3 translate ) : m_rotation(Quaternion::Identity), m_translation(translate) {}
        INLINE OrthogonalTransform( Quaternion rotate, Vector3 translate ) : m_rotation(rotate), m_translation(translate) {}
        INLINE OrthogonalTransform( const Matrix3& mat ) : m_rotation(mat), m_translation(Vector3::Zero) {}
        INLINE OrthogonalTransform( const Matrix3& mat, Vector3 translate ) : m_rotation(mat), m_translation(translate) {}
        INLINE static OrthogonalTransform Identity() { return OrthogonalTransform(Quaternion::Identity, Vector3::Zero); }
        INLINE explicit OrthogonalTransform( const XMMATRIX& mat ) { *this = OrthogonalTransform( Matrix3(mat), Vector3(mat.r[3]) ); }

        INLINE void SetRotation( Quaternion q ) { m_rotation = q; }
        INLINE void SetTranslation( Vector3 v ) { m_translation = v; }

        INLINE Quaternion GetRotation() const { return m_rotation; }
        INLINE Vector3 GetTranslation() const { return m_translation; }

        static INLINE OrthogonalTransform MakeXRotation( float angle ) { return OrthogonalTransform(Quaternion(Vector3::UnitX, angle)); }
        static INLINE OrthogonalTransform MakeYRotation( float angle ) { return OrthogonalTransform(Quaternion(Vector3::UnitY, angle)); }
        static INLINE OrthogonalTransform MakeZRotation( float angle ) { return OrthogonalTransform(Quaternion(Vector3::UnitZ, angle)); }
        static INLINE OrthogonalTransform MakeTranslation( Vector3 translate ) { return OrthogonalTransform(translate); }

        INLINE Vector4 operator* ( Vector4 vec ) const { return
            Vector4(SetWToZero(m_rotation * Vector3((XMVECTOR)vec))) +
            Vector4(SetWToOne(m_translation)) * vec.w;
        }
        INLINE OrthogonalTransform operator* ( const OrthogonalTransform& xform ) const {
            return OrthogonalTransform( m_rotation * xform.m_rotation, Vector3(xform.m_translation * m_rotation) + m_translation );
        }

        INLINE OrthogonalTransform operator~ () const { Quaternion invertedRotation = ~m_rotation;
            return OrthogonalTransform( invertedRotation, -m_translation * invertedRotation );
        }

    private:

        Quaternion m_rotation;
        Vector3 m_translation;
    };


    INLINE Vector3 operator* (const Vector3& vec, const OrthogonalTransform& O)
    {
       return Vector3(vec * O.GetRotation()) + O.GetTranslation();
    }

    inline Vector3 operator* (const Vector3& v, const class AffineTransform& A);

    // A AffineTransform is a 3x4 matrix with an implicit 4th row = [0,0,0,1].  This is used to perform a change of
    // basis on 3D points.  An affine transformation does not have to have orthonormal basis vectors.
    __declspec(align(64)) class AffineTransform
    {
    public:
        INLINE AffineTransform()
            {}
        INLINE AffineTransform( Vector3 x, Vector3 y, Vector3 z, Vector3 w )
            : m_basis(x, y, z), m_translation(w) {}
        INLINE AffineTransform( Vector3 translate )
            : m_basis(Matrix3::Identity()), m_translation(translate) {}
        INLINE AffineTransform( const Matrix3& mat, Vector3 translate = Vector3::Zero )
            : m_basis(mat), m_translation(translate) {}
        INLINE AffineTransform( Quaternion rot, Vector3 translate = Vector3::Zero )
            : m_basis(rot), m_translation(translate) {}
        INLINE AffineTransform( const OrthogonalTransform& xform )
            : m_basis(xform.GetRotation()), m_translation(xform.GetTranslation()) {}
        INLINE static AffineTransform Identity() { return AffineTransform(Matrix3::Identity(), Vector3::Zero); }
        INLINE explicit AffineTransform( const XMMATRIX& mat )
            : m_basis(mat), m_translation(mat.r[3]) {}

        INLINE operator XMMATRIX() const { return (XMMATRIX&)*this; }

        INLINE void SetX(Vector3 x) { m_basis.SetX(x); }
        INLINE void SetY(Vector3 y) { m_basis.SetY(y); }
        INLINE void SetZ(Vector3 z) { m_basis.SetZ(z); }
        INLINE void SetTranslation(Vector3 w) { m_translation = w; }

        INLINE Vector3 GetX() const { return m_basis.GetX(); }
        INLINE Vector3 GetY() const { return m_basis.GetY(); }
        INLINE Vector3 GetZ() const { return m_basis.GetZ(); }
        INLINE Vector3 GetTranslation() const { return m_translation; }
        INLINE const Matrix3& GetBasis() const { return (const Matrix3&)*this; }

        static INLINE AffineTransform MakeXRotation( float angle ) { return AffineTransform(Matrix3::MakeXRotation(angle)); }
        static INLINE AffineTransform MakeYRotation( float angle ) { return AffineTransform(Matrix3::MakeYRotation(angle)); }
        static INLINE AffineTransform MakeZRotation( float angle ) { return AffineTransform(Matrix3::MakeZRotation(angle)); }
        static INLINE AffineTransform MakeScale( float scale ) { return AffineTransform(Matrix3::MakeScale(scale)); }
        static INLINE AffineTransform MakeScale( Vector3 scale ) { return AffineTransform(Matrix3::MakeScale(scale)); }
        static INLINE AffineTransform MakeTranslation( Vector3 translate ) { return AffineTransform(translate); }

        INLINE AffineTransform operator* ( const AffineTransform& mat ) const {
            return AffineTransform(m_basis * mat.m_basis, mat.GetTranslation() * (*this));
        }

    private:
        Matrix3 m_basis;
        Vector3 m_translation;
    };

    inline Vector3 operator* (const Vector3& v, const AffineTransform& A) { return v * A.GetBasis() + A.GetTranslation(); }
}
