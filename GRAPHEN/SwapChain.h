#pragma once
#include "ColorBuffer.h"

class SwapChain
{
public:

   static void Initialize(uint32_t width, uint32_t height, DXGI_FORMAT format);
   static void Shutdown();
   static void Terminate();

   void Present(UINT SyncInterval, UINT Flags);
   void Resize(uint32_t width, uint32_t height);

   ColorBuffer& GetBuffer(UINT n);
   DXGI_FORMAT GetFormat() { return SwapChainFormat; }

   UINT GetBufferCount() const;

private:
   DXGI_FORMAT SwapChainFormat;

   UINT m_swapChainBufferCount;

   std::vector<ColorBuffer> m_displayPlane;
};

extern SwapChain* s_SwapChain;
