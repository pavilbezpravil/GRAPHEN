#pragma once
#include "gnpch.h"
#include "ColorBuffer.h"

class SwapChain
{
public:

   virtual ~SwapChain();

   void Initialize(HWND hwnd, uint32_t width, uint32_t height, DXGI_FORMAT format);
   void Shutdown();

   void WaitCurrentBackBuffer();

   void Present(UINT SyncInterval, UINT Flags);
   void Resize(uint32_t width, uint32_t height);

   ColorBuffer& GetCurrentBackBuffer();
   ColorBuffer& GetBuffer(UINT n);
   DXGI_FORMAT GetFormat() { return SwapChainFormat; }

   UINT GetBufferCount() const;

private:
   DXGI_FORMAT SwapChainFormat;

   UINT m_swapChainBufferCount;
   class IDXGISwapChain4* s_SwapChain1;

   std::vector<ColorBuffer> m_displayPlane;

   std::vector<UINT64> m_BackbufferFences;

   void InitDisplayPlanes();
};
