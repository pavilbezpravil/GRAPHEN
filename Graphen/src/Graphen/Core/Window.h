#pragma once

#include "gnpch.h"

#include "Graphen/Core/Core.h"
#include "Graphen/Events/Event.h"
#include "Graphen/Render/SwapChain.h"

namespace gn {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "gn Engine",
			        unsigned int width = 1280,
			        unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

      virtual void InitSwapChain() = 0;
      virtual void ShutdownSwapChain() = 0;

		virtual void OnHandleInput() = 0;

      virtual void Show(bool show = true) = 0;
      virtual void ShowCursor(bool show = true) = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

      virtual SwapChain& GetSwapChain() = 0;

		virtual void* GetNativeWindow() const = 0;
      virtual void Resize(UINT width, UINT height) = 0;

      static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}
