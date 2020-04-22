#pragma once

#include "BufferManager.h"

extern uint32_t g_DisplayWidth;
extern uint32_t g_DisplayHeight;
extern uint32_t g_NativeWidth;
extern uint32_t g_NativeHeight;
enum eResolution { k720p, k900p, k1080p, k1440p, k1800p, k2160p };

extern BoolVar s_EnableVSync;
extern EnumVar TargetResolution;

class Window
{
public:
   static void Initialize(const char* name, uint32_t width, uint32_t height);
   static void Terminate();

   void SetNativeResolution(void);

   static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

   const std::string& GetName() { return name; }
   uint32_t GetWidth() { return width; }
   uint32_t GetHeight() { return height; }
   HWND GetHWND() { return g_hWnd; }

   void ShowWindow(uint32_t showWindowCommand);
   void ShowCursor(bool show = true);

private:
   Window(const char* name, uint32_t width, uint32_t height);
   ~Window();

   std::string name;
   uint32_t width, height;
   HWND g_hWnd;

   void Resize(uint32_t width, uint32_t height);
};

extern Window* s_Window;
