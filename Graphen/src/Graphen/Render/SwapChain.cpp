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
   swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
   swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

   // Obtain the DXGI factory
   Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
   ASSERT_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

   ASSERT_SUCCEEDED(dxgiFactory->CreateSwapChainForHwnd(Graphics::g_CommandManager.GetCommandQueue(), hwnd, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&s_SwapChain1));

   m_displayPlane.resize(m_swapChainBufferCount);
   InitDisplayPlanes();

   m_BackbufferFences.resize(m_swapChainBufferCount);
}

void SwapChain::Shutdown()
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
   s_SwapChain1->SetFullscreenState(FALSE, nullptr);
#endif

   for (UINT i = 0; i < m_swapChainBufferCount; ++i)
      m_displayPlane[i].Destroy();

   SafeRelease(s_SwapChain1);
}

void SwapChain::Present(UINT SyncInterval, UINT Flags)
{
   auto& window = gn::Application::Get().GetWindow();

   {
      GraphicsContext& context = GraphicsContext::Begin(L"Backbuffer to Present");
      context.TransitionResource(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, true);
      context.Finish();
   }

   static int n = 0;
   // HZ_CORE_INFO("SwapChain::Present {0}", n++);

   // DXGI_PRESENT_PARAMETERS presentParameters;
   // ZeroMemory(&presentParameters, sizeof(presentParameters));
   // HZ_CORE_ASSERT(s_SwapChain1->Present1(SyncInterval, Flags, &presentParameters) == S_OK, "");
   HZ_CORE_ASSERT(s_SwapChain1->Present(SyncInterval, Flags) == S_OK, "");

   UINT backbufferIdx = s_SwapChain1->GetCurrentBackBufferIndex();
   Graphics::g_CommandManager.GetGraphicsQueue().WaitForFence(m_BackbufferFences[backbufferIdx]);
   m_BackbufferFences[backbufferIdx] = Graphics::g_CommandManager.GetGraphicsQueue().IncrementFence();

   {
      GraphicsContext& context = GraphicsContext::Begin(L"Backbuffer to RT");
      context.TransitionResource(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
      context.ClearColor(GetCurrentBackBuffer());
      context.Finish();
   }
}

void SwapChain::Resize(uint32_t width, uint32_t height)
{
   for (uint32_t i = 0; i < m_swapChainBufferCount; ++i)
      m_displayPlane[i].Destroy();

   ASSERT_SUCCEEDED(s_SwapChain1->ResizeBuffers(m_swapChainBufferCount, width, height, SwapChainFormat, 0));

   InitDisplayPlanes();
}

ColorBuffer& SwapChain::GetCurrentBackBuffer()
{
   return m_displayPlane[s_SwapChain1->GetCurrentBackBufferIndex()];
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
   for (uint32_t i = 0; i < m_swapChainBufferCount; ++i)
   {
      ComPtr<ID3D12Resource> DisplayPlane;
      ASSERT_SUCCEEDED(s_SwapChain1->GetBuffer(i, IID_PPV_ARGS(&DisplayPlane)));
      m_displayPlane[i].CreateFromSwapChain(std::wstring( L"Primary SwapChain Buffer ") + std::to_wstring(i), DisplayPlane.Detach());
   }
}
