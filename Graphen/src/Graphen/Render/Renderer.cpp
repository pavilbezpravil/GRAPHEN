#include "gnpch.h"
#include "Renderer.h"
#include "Graphen/Core/Application.h"
#include "Graphen/Render/CommandContext.h"
#include "Graphen/Render/RenderUtils.h"

namespace gn
{
   class RenderUtils;

   Renderer::Renderer()
         : m_width(0), m_height(0)
         , m_colorFormat(DXGI_FORMAT_R10G10B10A2_UNORM)
         , m_depthFormat(DXGI_FORMAT_D32_FLOAT) { }

   bool Renderer::Init(uint16_t width, uint16_t height) {
      return Resize(width, height);
   }

   void Renderer::Destroy()
   {
      m_colorBufferLDR.Destroy();
      m_depth.Destroy();
   }

   bool Renderer::Resize(uint16_t width, uint16_t height) {
      if (width == m_width && height == m_height) {
         return true;
      }

      m_width = width;
      m_height = height;

      m_colorBufferLDR.Create(L"Color Buffer LDR", m_width, m_height, 1, m_colorFormat);
      m_depth.Create(L"Depth Buffer", m_width, m_height, 1, m_depthFormat);
   }

   ColorBuffer& Renderer::GetColorBufferLDR()
   {
      return m_colorBufferLDR;
   }

   DepthBuffer& Renderer::GetDepth()
   {
      return m_depth;
   }

   void Renderer::Present()
   {
      auto& window = Application::Get().GetWindow();
      auto& swapchain = Application::Get().GetWindow().GetSwapChain();

      swapchain.WaitCurrentBackBuffer();

      GraphicsContext& context = GraphicsContext::Begin(L"Color Buffer to Back Buffer");
      context.TransitionResource(swapchain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);

      RenderUtils::DrawFullScreenQuad(context, swapchain.GetCurrentBackBuffer(), GetColorBufferLDR());

      context.TransitionResource(swapchain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, true);
      context.Finish();

      swapchain.Present(window.IsVSync(), 0);
   }
}
