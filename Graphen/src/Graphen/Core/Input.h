#pragma once

#include "Graphen/Core/Core.h"
#include "Graphen/Core/KeyCodes.h"
#include "Graphen/Core/MouseCodes.h"

namespace gn {

	class Input
	{
	protected:
	public:
		Input();
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

      static bool IsKeyPressed(KeyCode key);

      static bool IsMouseButtonPressed(MouseCode button);
      static Vector2 GetMousePosition();
      static Vector2 GetMouseAnalog();

      static void SetKeyboardPressed(KeyCode code, bool pressed);
      static void SetMousePressed(MouseCode code, bool pressed);
      static void SetMousePos(Vector2 pos);
      static void Update(float dt);

		static Scope<Input> Create();
	private:
		static Scope<Input> s_Instance;

      Vector2 m_PrevMousePos;
      Vector2 m_CurMousePos;
      Vector2 m_MouseDiff;

      Vector2 GetNativeMousePos();
      bool GetNativeKeyState(int key);

      bool m_KeyStates[1024]; // todo
      bool m_MouseStates[16]; // todo:
   };
}
