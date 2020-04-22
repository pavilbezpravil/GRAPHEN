#include "pch.h"
#include "Window.h"
#include "CommandContext.h"
#include "VidDriver.h"
#include "SwapChain.h"


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx12.h"

Window* s_Window;

#ifndef RELEASE
// const GUID WKPDID_D3DDebugObjectName = { 0x429b8c22,0x9188,0x4b0c, { 0x87,0x42,0xac,0xb0,0xbf,0x85,0xc2,0x00 } };
#endif

const uint32_t kMaxNativeWidth = 3840;
const uint32_t kMaxNativeHeight = 2160;
const uint32_t kNumPredefinedResolutions = 6;

const char* ResolutionLabels[] = { "1280x720", "1600x900", "1920x1080", "2560x1440", "3200x1800", "3840x2160" };
EnumVar TargetResolution("Graphics/Display/Native Resolution", k1080p, kNumPredefinedResolutions, ResolutionLabels);

BoolVar s_EnableVSync("Timing/VSync", true);

uint32_t g_NativeWidth = 0;
uint32_t g_NativeHeight = 0;
uint32_t g_DisplayWidth = 1920;
uint32_t g_DisplayHeight = 1080;

void Window::Initialize(const char* name, uint32_t width, uint32_t height)
{
   s_Window = new Window(name, width, height);

   HINSTANCE hInst = GetModuleHandle(0);

   auto className = MakeWStr(s_Window->name);

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
   ASSERT(0 != RegisterClassEx(&wcex), "Unable to register a window");

   g_DisplayWidth = s_Window->width;
   g_DisplayHeight = s_Window->height;

   // Create window
   RECT rc = { 0, 0, (LONG)s_Window->width, (LONG)s_Window->height };
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

   s_Window->g_hWnd = CreateWindow(className.data(), className.data(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
      rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
   ASSERT(s_Window->g_hWnd != 0);
}

void Window::Terminate()
{
   SafeDelete(s_Window);
}

void Window::SetNativeResolution()
{
   uint32_t NativeWidth, NativeHeight;

   switch (eResolution((int)TargetResolution))
   {
   default:
   case k720p:
      NativeWidth = 1280;
      NativeHeight = 720;
      break;
   case k900p:
      NativeWidth = 1600;
      NativeHeight = 900;
      break;
   case k1080p:
      NativeWidth = 1920;
      NativeHeight = 1080;
      break;
   case k1440p:
      NativeWidth = 2560;
      NativeHeight = 1440;
      break;
   case k1800p:
      NativeWidth = 3200;
      NativeHeight = 1800;
      break;
   case k2160p:
      NativeWidth = 3840;
      NativeHeight = 2160;
      break;
   }

   if (g_NativeWidth == NativeWidth && g_NativeHeight == NativeHeight)
      return;

   DEBUGPRINT("Changing native resolution to %ux%u", NativeWidth, NativeHeight);

   g_NativeWidth = NativeWidth;
   g_NativeHeight = NativeHeight;

   Graphics::g_CommandManager.IdleGPU();

   Graphics::InitializeRenderingBuffers(NativeWidth, NativeHeight);
}

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
      return true;

   switch (message)
   {
   case WM_SIZE:
      ImGui_ImplDX12_InvalidateDeviceObjects();
      s_Window->Resize((UINT)(UINT64)lParam & 0xFFFF, (UINT)(UINT64)lParam >> 16);
      ImGui_ImplDX12_CreateDeviceObjects();
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }

   return 0;
}

void Window::ShowWindow(uint32_t showWindowCommand)
{
   ::ShowWindow(g_hWnd, showWindowCommand);
}

void Window::ShowCursor(bool show)
{
   if (show) {
      while (::ShowCursor(TRUE) < 0);
   } else {
      while (::ShowCursor(FALSE) >= 0);
   }
}

Window::Window(const char* name, uint32_t width, uint32_t height)
   : name(name), width(width), height(height)
{
}

Window::~Window()
{
   // todo:
}

void Window::Resize(uint32_t width, uint32_t height)
{
   // Check for invalid window dimensions
   if (width == 0 || height == 0)
      return;

   // Check for an unneeded resize
   if (width == g_DisplayWidth && height == g_DisplayHeight)
      return;

   Graphics::g_CommandManager.IdleGPU();

   this->width = width;
   this->height = height;

   g_DisplayWidth = width;
   g_DisplayHeight = height;

   DEBUGPRINT("Changing display resolution to %ux%u", width, height);

   s_SwapChain->Resize(width, height);

   Graphics::g_CommandManager.IdleGPU();

   Graphics::ResizeDisplayDependentBuffers(g_NativeWidth, g_NativeHeight);
}
