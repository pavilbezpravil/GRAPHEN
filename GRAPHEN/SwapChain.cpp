#include "pch.h"
#include "SwapChain.h"
#include "ColorBuffer.h"
#include "VidDriver.h"
#include "Window.h"
#include <dxgi1_2.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>

#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

SwapChain* s_SwapChain;

IDXGISwapChain1* s_SwapChain1;

void SwapChain::Initialize(uint32_t width, uint32_t height, DXGI_FORMAT format)
{
   s_SwapChain = new SwapChain();
   s_SwapChain->SwapChainFormat = format;
   s_SwapChain->m_swapChainBufferCount = 3;

   DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
   swapChainDesc.Width = width;
   swapChainDesc.Height = height;
   swapChainDesc.Format = format;
   swapChainDesc.Scaling = DXGI_SCALING_NONE;
   swapChainDesc.SampleDesc.Quality = 0;
   swapChainDesc.SampleDesc.Count = 1;
   swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   swapChainDesc.BufferCount = s_SwapChain->m_swapChainBufferCount;
   swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
   swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

   // Obtain the DXGI factory
   Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
   ASSERT_SUCCEEDED(CreateDXGIFactory2(0, MY_IID_PPV_ARGS(&dxgiFactory)));

   ASSERT_SUCCEEDED(dxgiFactory->CreateSwapChainForHwnd(Graphics::g_CommandManager.GetCommandQueue(), s_Window->GetHWND(), &swapChainDesc, nullptr, nullptr, &s_SwapChain1));

#if CONDITIONALLY_ENABLE_HDR_OUTPUT && defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
   {
      IDXGISwapChain4* swapChain = (IDXGISwapChain4*)s_SwapChain1;
      Microsoft::WRL::ComPtr<IDXGIOutput> output;
      Microsoft::WRL::ComPtr<IDXGIOutput6> output6;
      DXGI_OUTPUT_DESC1 outputDesc;
      UINT colorSpaceSupport;

      // Query support for ST.2084 on the display and set the color space accordingly
      if (SUCCEEDED(swapChain->GetContainingOutput(&output)) &&
         SUCCEEDED(output.As(&output6)) &&
         SUCCEEDED(output6->GetDesc1(&outputDesc)) &&
         outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 &&
         SUCCEEDED(swapChain->CheckColorSpaceSupport(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020, &colorSpaceSupport)) &&
         (colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) &&
         SUCCEEDED(swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)))
      {
         // g_bEnableHDROutput = true;
      }
   }
#endif

   s_SwapChain->m_displayPlane.resize(s_SwapChain->m_swapChainBufferCount);
   for (uint32_t i = 0; i < s_SwapChain->m_swapChainBufferCount; ++i)
   {
      Microsoft::WRL::ComPtr<ID3D12Resource> DisplayPlane;
      ASSERT_SUCCEEDED(s_SwapChain1->GetBuffer(i, MY_IID_PPV_ARGS(&DisplayPlane)));
      s_SwapChain->m_displayPlane[i].CreateFromSwapChain(L"Primary SwapChain Buffer", DisplayPlane.Detach());
   }
}

void SwapChain::Shutdown()
{
   for (UINT i = 0; i < s_SwapChain->m_swapChainBufferCount; ++i)
      s_SwapChain->m_displayPlane[i].Destroy();

   SafeRelease(s_SwapChain1);
}

void SwapChain::Terminate()
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
   s_SwapChain1->SetFullscreenState(FALSE, nullptr);
#endif
}

void SwapChain::Present(UINT SyncInterval, UINT Flags)
{
   s_SwapChain1->Present(SyncInterval, Flags);
}

void SwapChain::Resize(uint32_t width, uint32_t height)
{
   for (uint32_t i = 0; i < s_SwapChain->m_swapChainBufferCount; ++i)
      s_SwapChain->m_displayPlane[i].Destroy();

   ASSERT_SUCCEEDED(s_SwapChain1->ResizeBuffers(s_SwapChain->m_swapChainBufferCount, width, height, SwapChainFormat, 0));

   for (uint32_t i = 0; i < s_SwapChain->m_swapChainBufferCount; ++i)
   {
      Microsoft::WRL::ComPtr<ID3D12Resource> DisplayPlane;
      ASSERT_SUCCEEDED(s_SwapChain1->GetBuffer(i, MY_IID_PPV_ARGS(&DisplayPlane)));
      s_SwapChain->m_displayPlane[i].CreateFromSwapChain(L"Primary SwapChain Buffer", DisplayPlane.Detach());
   }
}

ColorBuffer& SwapChain::GetBuffer(UINT n)
{
   return m_displayPlane[n];
}

UINT SwapChain::GetBufferCount() const
{
   return m_swapChainBufferCount;
}
