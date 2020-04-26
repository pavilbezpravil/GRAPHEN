#include "gnpch.h"
#include "Graphen/Core/Input.h"
#include "Application.h"

namespace gn {

	Scope<Input> Input::s_Instance = Input::Create();

   Input::Input() : m_PrevMousePos(FLT_MAX, FLT_MAX), m_CurMousePos(FLT_MAX, FLT_MAX), m_MouseDiff(FLT_MAX, FLT_MAX)
   {
      ZeroMemory(m_KeyStates, sizeof(m_KeyStates));
      ZeroMemory(m_MouseStates, sizeof(m_MouseStates));
   }

   bool Input::IsKeyPressed(KeyCode key)
   {
      return s_Instance->m_KeyStates[(int)key];
      // return GetKeyState((int)key) >> 15;
   }

   bool Input::IsMouseButtonPressed(MouseCode button)
   {
      return s_Instance->m_MouseStates[(int)button];
      // return GetKeyState((int)button) >> 15;
   }

   Vector2 Input::GetMousePosition()
   {
      if (s_Instance->m_CurMousePos.x == FLT_MAX)
      {
         s_Instance->m_PrevMousePos = s_Instance->m_CurMousePos = s_Instance->GetNativeMousePos();
      }
      return s_Instance->m_CurMousePos;
   }

   Vector2 Input::GetMouseAnalog()
   {
      return s_Instance->m_MouseDiff;
   }

   void Input::SetKeyboardPressed(KeyCode code, bool pressed)
   {
      s_Instance->m_KeyStates[(int)code] = pressed;
   }

   void Input::SetMousePressed(MouseCode code, bool pressed)
   {
      s_Instance->m_MouseStates[(int)code] = pressed;
   }

   void Input::SetMousePos(Vector2 pos)
   {
      s_Instance->m_CurMousePos = pos;
   }

   void Input::Update(float dt)
   {
      s_Instance->m_MouseDiff = (s_Instance->GetMousePosition() - s_Instance->m_PrevMousePos) * dt;
   }


   Scope<Input> Input::Create()
	{
		return CreateScope<Input>();
	}

   Vector2 Input::GetNativeMousePos()
   {
      HWND hWnd = *(HWND*)Application::Get().GetWindow().GetNativeWindow();
      POINT p;
      if (GetCursorPos(&p))
      {
         //cursor position now in p.x and p.y
         if (ScreenToClient(hWnd, &p))
         {
            //p.x and p.y are now relative to hwnd's client area
            return Vector2{ (float)p.x, (float)p.y };
         }
      }
      return { 0, 0 };
   }

   bool Input::GetNativeKeyState(int key)
   {
      return ::GetKeyState((int)key) >> 15;
   }
} 