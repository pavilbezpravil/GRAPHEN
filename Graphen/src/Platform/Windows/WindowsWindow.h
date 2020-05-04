#pragma once

#include "Graphen/Core/Window.h"
#include "Graphen/Render/SwapChain.h"

namespace gn {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

      void InitSwapChain() override;
      void ShutdownSwapChain() override;

      void OnHandleInput() override;

      void Show(bool show) override;
      void ShowCursor(bool show) override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

      SwapChain& GetSwapChain() override;

      void Resize(UINT width, UINT height) override;
		virtual void* GetNativeWindow() const { return (void*)&m_HWND; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		HWND m_HWND;
      uptr<SwapChain> m_SwapChain;
      bool m_mouseShow;

      static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}
