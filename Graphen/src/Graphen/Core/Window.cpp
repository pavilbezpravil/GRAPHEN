#include "gnpch.h"
#include "Graphen/Core/Window.h"

#ifdef HZ_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace gn
{

	Scope<Window> Window::Create(const WindowProps& props)
	{
	#ifdef HZ_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
	#else
		GN_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}

}