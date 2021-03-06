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
#include "CameraController.h"
#include "Camera.h"
#include "Graphen/Core/Input.h"

using namespace Math;
using namespace GameCore;
using namespace gn;

CameraController::CameraController( Camera& camera, Vector3 worldUp ) : m_TargetCamera( camera )
{
    m_WorldUp = Normalize(worldUp);
    m_WorldNorth = Normalize(Cross(m_WorldUp, Vector3::UnitX));
    m_WorldEast = Cross(m_WorldNorth, m_WorldUp);

    m_HorizontalLookSensitivity = 2.0f;
    m_VerticalLookSensitivity = 2.0f;
    m_MoveSpeed = 1.0f;
    m_StrafeSpeed = 1.0f;
    m_MouseSensitivityY = m_MouseSensitivityX = .3f;

    m_CurrentPitch = Sin(Dot(camera.GetForwardVec(), m_WorldUp));

    Vector3 forward = Normalize(Cross(m_WorldUp, camera.GetRightVec()));
    m_CurrentHeading = ATan2(-Dot(forward, m_WorldEast), Dot(forward, m_WorldNorth));

    m_FineMovement = false;
    m_FineRotation = false;
    m_Momentum = true;

    m_Enable = true;

    m_LastYaw = 0.0f;
    m_LastPitch = 0.0f;
    m_LastForward = 0.0f;
    m_LastStrafe = 0.0f;
    m_LastAscent = 0.0f;
}

void CameraController::Update( float deltaTime )
{
   if (!m_Enable) {
      return;
   }
   
    (deltaTime);
   
    float timeScale = 1.0f;
   
    if (Input::IsKeyPressed(HZ_KEY_LEFT_SHIFT))
        m_FineMovement = !m_FineMovement;
   
    // if (GameInput::IsFirstPressed(GameInput::kRThumbClick))
        // m_FineRotation = !m_FineRotation;
   
    float speedScale = (m_FineMovement ? 0.1f : 1.0f) * timeScale;
    float panScale = (m_FineRotation ? 0.5f : 1.0f) * timeScale;
   
    // float yaw = GameInput::GetTimeCorrectedAnalogInput( GameInput::kAnalogRightStickX ) * m_HorizontalLookSensitivity * panScale;
    // float pitch = GameInput::GetTimeCorrectedAnalogInput( GameInput::kAnalogRightStickY ) * m_VerticalLookSensitivity * panScale;
    float yaw = 0.f;
    float pitch = 0.f;
    float forward = m_MoveSpeed * speedScale * (
        (Input::IsKeyPressed(HZ_KEY_W) ? deltaTime : 0.0f) +
        (Input::IsKeyPressed(HZ_KEY_S) ? -deltaTime : 0.0f)
        );
    float strafe = m_StrafeSpeed * speedScale * (
        (Input::IsKeyPressed(HZ_KEY_D) ? deltaTime : 0.0f) +
        (Input::IsKeyPressed(HZ_KEY_A) ? -deltaTime : 0.0f)
        );
    float ascent = m_StrafeSpeed * speedScale * (
        (Input::IsKeyPressed(HZ_KEY_E) ? deltaTime : 0.0f) +
        (Input::IsKeyPressed(HZ_KEY_Q) ? -deltaTime : 0.0f)
        );
   
    if (m_Momentum)
    {
        ApplyMomentum(m_LastYaw, yaw, deltaTime);
        ApplyMomentum(m_LastPitch, pitch, deltaTime);
        ApplyMomentum(m_LastForward, forward, deltaTime);
        ApplyMomentum(m_LastStrafe, strafe, deltaTime);
        ApplyMomentum(m_LastAscent, ascent, deltaTime);
    }
   
    // don't apply momentum to mouse inputs
    Vector2 mouseAnalog = Input::GetMouseAnalog();
    yaw += mouseAnalog.x * m_MouseSensitivityX;
    pitch += mouseAnalog.y * m_MouseSensitivityY;
   
    m_CurrentPitch += pitch;
    m_CurrentPitch = XMMin( XM_PIDIV2, m_CurrentPitch);
    m_CurrentPitch = XMMax(-XM_PIDIV2, m_CurrentPitch);
   
    m_CurrentHeading -= yaw;
    if (m_CurrentHeading > XM_PI)
        m_CurrentHeading -= XM_2PI;
    else if (m_CurrentHeading <= -XM_PI)
        m_CurrentHeading += XM_2PI; 
   
    Matrix3 orientation = Matrix3::MakeXRotation(m_CurrentPitch) * Matrix3::MakeYRotation(m_CurrentHeading) * Matrix3(m_WorldEast, m_WorldUp, -m_WorldNorth);
    Vector3 position = Vector3( strafe, ascent, -forward ) * orientation + m_TargetCamera.GetPosition();
    m_TargetCamera.SetTransform( AffineTransform( orientation, position ) );
    m_TargetCamera.Update();
}

void CameraController::ApplyMomentum( float& oldValue, float& newValue, float deltaTime )
{
    float blendedValue;
    if (Abs(newValue) > Abs(oldValue))
        blendedValue = Lerp(newValue, oldValue, Pow(0.6f, deltaTime * 60.0f));
    else
        blendedValue = Lerp(newValue, oldValue, Pow(0.8f, deltaTime * 60.0f));
    oldValue = blendedValue;
    newValue = blendedValue;
}
