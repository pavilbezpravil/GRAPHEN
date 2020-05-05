#include "gnpch.h"
#include "Renderer.h"
#include "Graphen/Core/Application.h"
#include "Graphen/Render/CommandContext.h"
#include "Graphen/Render/RenderUtils.h"
#include "Graphen/Render/Scene.h"
#include "Graphen/Core/Camera.h"

namespace gn {
   Renderer::Renderer()
         : m_width(0), m_height(0)
         , m_colorFormat(DXGI_FORMAT_R10G10B10A2_UNORM)
         , m_depthFormat(DXGI_FORMAT_D32_FLOAT)
         , m_curLDRTarget(0) { }

   bool Renderer::Init(uint16_t width, uint16_t height) {
      return Resize(width, height);
   }

   void Renderer::Destroy()
   {
      m_colorBufferLDR[0].Destroy();
      m_colorBufferLDR[1].Destroy();
      m_depth.Destroy();
   }

   bool Renderer::Resize(uint16_t width, uint16_t height) {
      if (width == m_width && height == m_height) {
         return true;
      }

      m_width = width;
      m_height = height;

      m_colorBufferLDR[0].Create(L"Color Buffer LDR 0", m_width, m_height, 1, m_colorFormat);
      m_colorBufferLDR[1].Create(L"Color Buffer LDR 1", m_width, m_height, 1, m_colorFormat);
      m_depth.Create(L"Depth Buffer", m_width, m_height, 1, m_depthFormat);
   }

   void Renderer::DrawScene(const Scene& scene, const Camera& camera) {
      HZ_PROFILE_FUNCTION();

      ColorBuffer& colorBuffer = GetLDRTarget();
      DepthBuffer& depthBuffer = GetDepth();

      GraphicsContext& context = GraphicsContext::Begin(L"DrawScene");

      
      context.TransitionResource(depthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
      context.ClearDepth(depthBuffer);

      context.SetRenderTarget(colorBuffer.GetRTV(), depthBuffer.GetDSV());
      context.SetViewportAndScissor(0, 0, colorBuffer.GetWidth(), colorBuffer.GetHeight());

      {
         GPU_EVENT_SCOPE("ZPass");
         scene.Draw(context, camera, PASS_NAME_Z_PASS);
      }
      
      context.TransitionResource(depthBuffer, D3D12_RESOURCE_STATE_DEPTH_READ);
      context.TransitionResource(colorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
      context.ClearColor(colorBuffer);

      {
         GPU_EVENT_SCOPE("Opaque");
         scene.Draw(context, camera, PASS_NAME_OPAQUE);
      }
      
      context.Finish();
   }

   ColorBuffer& Renderer::GetLDRTarget()
   {
      return m_colorBufferLDR[m_curLDRTarget];
   }

   ColorBuffer& Renderer::GetLDRBB() {
      return m_colorBufferLDR[GetBBIndex()];
   }

   DepthBuffer& Renderer::GetDepth()
   {
      return m_depth;
   }

   void Renderer::SwapLDRBuffer() {
      m_curLDRTarget = GetBBIndex();
   }

   void Renderer::Present()
   {
      auto& window = Application::Get().GetWindow();
      auto& swapchain = Application::Get().GetWindow().GetSwapChain();

      swapchain.WaitCurrentBackBuffer();

      GraphicsContext& context = GraphicsContext::Begin(L"Color Buffer to Back Buffer");
      context.TransitionResource(swapchain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);

      RenderUtils::DrawFullScreenQuad(context, swapchain.GetCurrentBackBuffer(), GetLDRTarget());

      context.TransitionResource(swapchain.GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, true);
      context.Finish();

      swapchain.Present(window.IsVSync(), 0);

      ResetLDRIndex();
   }

   void Renderer::ResetLDRIndex() {
      m_curLDRTarget = 0;
   }

   uint8 Renderer::GetBBIndex() const {
      return 1 - m_curLDRTarget;
   }

   uint8 Renderer::GetTargetIndex() const {
      return m_curLDRTarget;
   }
}
