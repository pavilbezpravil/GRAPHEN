#include "gnpch.h"

#include "Platform/Windows/WindowsWindow.h"

#include "Graphen/Core/Input.h"

#include "Graphen/Events/ApplicationEvent.h"
#include "Graphen/Events/MouseEvent.h"
#include "Graphen/Events/KeyEvent.h"

#include "Graphen/Core/Application.h"
#include <WinUser.h>
#include <windowsx.h>
#include "Graphen/Render/CommandContext.h"


// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace gn {

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		HZ_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		HZ_PROFILE_FUNCTION();

      m_SwapChain = nullptr;
		Shutdown();
	}

   void WindowsWindow::InitSwapChain()
   {
      m_SwapChain = std::make_unique<SwapChain>();
      m_SwapChain->Initialize(m_HWND, m_Data.Width, m_Data.Height, DXGI_FORMAT_R10G10B10A2_UNORM);
   }

   void WindowsWindow::ShutdownSwapChain()
   {
      m_SwapChain = nullptr;
   }

   void WindowsWindow::OnHandleInput() {
      HZ_PROFILE_FUNCTION();

      MSG msg = {};
      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }

      if (!m_mouseShow) {
         RECT rect;
         GetClientRect(m_HWND, &rect);

         POINT ul;
         ul.x = rect.left;
         ul.y = rect.top;

         POINT lr;
         lr.x = rect.right;
         lr.y = rect.bottom;

         MapWindowPoints(m_HWND, nullptr, &ul, 1);
         MapWindowPoints(m_HWND, nullptr, &lr, 1);

         rect.left = ul.x;
         rect.top = ul.y;

         rect.right = lr.x;
         rect.bottom = lr.y;

         ClipCursor(&rect);
      } else {
         ClipCursor(nullptr);
      }
	}

   void WindowsWindow::Init(const WindowProps& props)
	{
		HZ_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		GN_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

      HINSTANCE hInst = GetModuleHandle(0);

      auto className = MakeWStr(m_Data.Title);

      // Register class
      WNDCLASSEX wcex;
      wcex.cbSize = sizeof(WNDCLASSEX);
      wcex.style = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc = WndProc;
      wcex.cbClsExtra = 0;
      wcex.cbWndExtra = 0;
      wcex.hInstance = hInst;
      wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);
      wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
      wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
      wcex.lpszMenuName = nullptr;
      wcex.lpszClassName = className.data();
      wcex.hIconSm = LoadIcon(hInst, IDI_APPLICATION);
      GN_CORE_ASSERT(RegisterClassEx(&wcex), "Unable to register a window");

      // Create window
      RECT rc = { 0, 0, (LONG)m_Data.Width, (LONG)m_Data.Height };
      AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

      m_HWND = CreateWindow(className.data(), className.data(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
         rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
      // GN_CORE_ASSERT(m_HWND);

      SetWindowLongPtr(m_HWND, GWLP_USERDATA, (LONG_PTR)&this->m_Data);

		SetVSync(true);


		//
		// glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		// {
		// 	WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		//
		// 	switch (action)
		// 	{
		// 		case GLFW_PRESS:
		// 		{
		// 			KeyPressedEvent event(static_cast<KeyCode>(key), 0);
		// 			data.EventCallback(event);
		// 			break;
		// 		}
		// 		case GLFW_RELEASE:
		// 		{
		// 			KeyReleasedEvent event(static_cast<KeyCode>(key));
		// 			data.EventCallback(event);
		// 			break;
		// 		}
		// 		case GLFW_REPEAT:
		// 		{
		// 			KeyPressedEvent event(static_cast<KeyCode>(key), 1);
		// 			data.EventCallback(event);
		// 			break;
		// 		}
		// 	}
		// });
		//
		// glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		// {
		// 	WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		//
		// 	KeyTypedEvent event(static_cast<KeyCode>(keycode));
		// 	data.EventCallback(event);
		// });
		//

		// glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
		// {
		// 	WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		//
		// 	MouseScrolledEvent event((float)xOffset, (float)yOffset);
		// 	data.EventCallback(event);
		// });

	}

   LRESULT WindowsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
      if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
         return true;

      WindowData& data = *((WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA));

      GN_CORE_INFO("WndProc msg={}, wParam={}, lParam={}", msg, wParam, lParam);

      switch (msg)
      {
      case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
      case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
      case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
      {
         MouseCode button = MouseCode::ButtonLast;
         if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = MouseCode::ButtonLeft; }
         if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = MouseCode::ButtonRight; }
         if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = MouseCode::ButtonMiddle; }

         if (button != MouseCode::ButtonLast)
         {
            Input::SetMousePressed(static_cast<MouseCode>(button), true);
            data.EventCallback(MouseButtonPressedEvent(static_cast<MouseCode>(button)));
         }
         break;
      }
      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
      case WM_MBUTTONUP:
      {
         MouseCode button = MouseCode::ButtonLast;
         if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = MouseCode::ButtonLeft; }
         if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = MouseCode::ButtonRight; }
         if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = MouseCode::ButtonMiddle; }
         if (button != MouseCode::ButtonLast)
         {
            Input::SetMousePressed(static_cast<MouseCode>(button), false);
            data.EventCallback(MouseButtonReleasedEvent(static_cast<MouseCode>(button)));
         }
         break;
      }
      case WM_SIZE:
         data.EventCallback(WindowResizeEvent((UINT)(UINT64)lParam & 0xFFFF, (UINT)(UINT64)lParam >> 16));
         break;

      case WM_KEYDOWN:
      // case WM_CHAR:
         Input::SetKeyboardPressed(static_cast<KeyCode>(wParam), true);
         data.EventCallback(KeyPressedEvent(static_cast<KeyCode>(wParam), 1));
         break;

      case WM_KEYUP:
         Input::SetKeyboardPressed(static_cast<KeyCode>(wParam), false);
         data.EventCallback(KeyReleasedEvent(static_cast<KeyCode>(wParam)));
         break;

      case WM_MOUSEMOVE:
         Input::SetMousePos({ (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) });
         data.EventCallback(MouseMovedEvent((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)));
         break;
      case WM_DESTROY:
         data.EventCallback(WindowCloseEvent {});
         PostQuitMessage(0);
         break;

      default:
         return DefWindowProc(hWnd, msg, wParam, lParam);
      }

      return 0;
   }

	void WindowsWindow::Shutdown()
	{
		HZ_PROFILE_FUNCTION();

      // todo:
	}

   void WindowsWindow::Show(bool show)
   {
      ::ShowWindow(m_HWND, SW_SHOWDEFAULT);
   }

   void WindowsWindow::ShowCursor(bool show)
   {
      if (show) {
         while (::ShowCursor(true) < 0);
      } else {
         while (::ShowCursor(false) >= 0);
      }
      m_mouseShow = show;
   }

   void WindowsWindow::SetVSync(bool enabled)
	{
		HZ_PROFILE_FUNCTION();

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

   SwapChain& WindowsWindow::GetSwapChain()
   {
      return *m_SwapChain;
   }

   void WindowsWindow::Resize(UINT width, UINT height)
   {
      m_Data.Width = width;
      m_Data.Height = height;
      m_SwapChain->Resize(width, height);
   }
}
