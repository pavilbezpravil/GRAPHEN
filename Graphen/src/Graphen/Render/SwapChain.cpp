#include "gnpch.h"
#include "SwapChain.h"
#include "ColorBuffer.h"
#include "VidDriver.h"
#include <dxgi1_2.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include "Graphen/Core/Application.h"

#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

SwapChain::~SwapChain()
{
   Shutdown();
}

void SwapChain::Initialize(HWND hwnd, uint32_t width, uint32_t height, DXGI_FORMAT format)
{
   SwapChainFormat = format;
   m_swapChainBufferCount = 3;

   DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
   swapChainDesc.Width = width;
   swapChainDesc.Height = height;
   swapChainDesc.Format = SwapChainFormat;
   swapChainDesc.Scaling = DXGI_SCALING_NONE;
   swapChainDesc.SampleDesc.Quality = 0;
   swapChainDesc.SampleDesc.Count = 1;
   swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   swapChainDesc.BufferCount = m_swapChainBufferCount;
   swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH /*| DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT*/;
   swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

   // Obtain the DXGI factory
   Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
   ASSERT_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

   ASSERT_SUCCEEDED(dxgiFactory->CreateSwapChainForHwnd(Graphics::g_CommandManager.GetCommandQueue(), hwnd, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&m_swapChain1));

   InitDisplayPlanes();

   m_BackbufferFences.resize(m_swapChainBufferCount);
}

void SwapChain::Shutdown()
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
   m_swapChain1->SetFullscreenState(FALSE, nullptr);
#endif

   for (UINT i = 0; i < m_swapChainBufferCount; ++i)
      m_displayPlane[i].Destroy();

   SafeRelease(m_swapChain1);

   // CloseHandle(m_swapChainWaitableObject);
   // m_swapChainWaitableObject = nullptr;
}

void SwapChain::WaitCurrentBackBuffer()
{
   UINT backbufferIdx = m_swapChain1->GetCurrentBackBufferIndex();
   Graphics::g_CommandManager.GetGraphicsQueue().WaitForFence(m_BackbufferFences[backbufferIdx]);
   // WaitForSingleObject(m_swapChainWaitableObject, INFINITE);
}

void SwapChain::Present(UINT SyncInterval, UINT Flags)
{
   UINT prevBackbufferIdx = m_swapChain1->GetCurrentBackBufferIndex();
   auto hr = m_swapChain1->Present(SyncInterval, Flags);
   ASSERT_SUCCEEDED(hr);
   
   m_BackbufferFences[prevBackbufferIdx] = Graphics::g_CommandManager.GetGraphicsQueue().IncrementFence();
}

void SwapChain::Resize(uint32_t width, uint32_t height)
{
   for (uint32_t i = 0; i < m_swapChainBufferCount; ++i)
      m_displayPlane[i].Destroy();

   // WaitForSingleObject(m_swapChainWaitableObject, INFINITE);
   // CloseHandle(m_swapChainWaitableObject);

   ASSERT_SUCCEEDED(m_swapChain1->ResizeBuffers(m_swapChainBufferCount, width, height, SwapChainFormat, 0));

   InitDisplayPlanes();
}

ColorBuffer& SwapChain::GetCurrentBackBuffer()
{
   return m_displayPlane[m_swapChain1->GetCurrentBackBufferIndex()];
}

ColorBuffer& SwapChain::GetBuffer(UINT n)
{
   return m_displayPlane[n];
}

UINT SwapChain::GetBufferCount() const
{
   return m_swapChainBufferCount;
}

void SwapChain::InitDisplayPlanes()
{
   m_displayPlane.resize(m_swapChainBufferCount);

   for (uint32_t i = 0; i < m_swapChainBufferCount; ++i)
   {
      ComPtr<ID3D12Resource> DisplayPlane;
      ASSERT_SUCCEEDED(m_swapChain1->GetBuffer(i, IID_PPV_ARGS(&DisplayPlane)));
      m_displayPlane[i].CreateFromSwapChain(std::wstring( L"Primary SwapChain Buffer ") + std::to_wstring(i), DisplayPlane.Detach());
   }

   // m_swapChainWaitableObject = m_swapChain1->GetFrameLatencyWaitableObject();
}
