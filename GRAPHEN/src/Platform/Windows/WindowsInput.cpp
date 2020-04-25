#include "gnpch.h"
#include "Platform/Windows/WindowsInput.h"

#include "Graphen/Core/Application.h"

namespace gn {

	bool WindowsInput::IsKeyPressedImpl(KeyCode key)
	{
		// auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		// auto state = glfwGetKey(window, static_cast<int32_t>(key));
		// return state == GLFW_PRESS || state == GLFW_REPEAT;
      return GetKeyState((int)key) >> 15;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(MouseCode button)
	{
		// auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		// auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		// return state == GLFW_PRESS;
      return GetKeyState((int)button) >> 15;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		// auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		// double xpos, ypos;
		// glfwGetCursorPos(window, &xpos, &ypos);
		//
		// return { (float)xpos, (float)ypos };
      HWND hWnd = *(HWND*)Application::Get().GetWindow().GetNativeWindow();
      POINT p;
      if (GetCursorPos(&p))
      {
         //cursor position now in p.x and p.y
         if (ScreenToClient(hWnd, &p))
         {
            //p.x and p.y are now relative to hwnd's client area
            return { (float)p.x, (float)p.y };
         }
      }

		return { (float)0, (float)0 };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePositionImpl();
		return y;
	}

}