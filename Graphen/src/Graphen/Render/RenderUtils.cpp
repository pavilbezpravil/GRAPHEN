#include "gnpch.h"
#include "RenderUtils.h"
#include "Graphen/Core/Application.h"
#include "Graphen/Render/GraphicsCommon.h"
#include "Graphen/Render/SamplerManager.h"

namespace gn
{
   RenderUtils* RenderUtils::s_instance = nullptr;

   RenderUtils::RenderUtils()
   {
      s_instance = this;
      m_inited = false;
   }

   bool RenderUtils::Initialize()
   {
      s_instance = new RenderUtils();
      return s_instance->BuildShadersAndPSO();
   }

   void RenderUtils::Shutdown()
   {
      delete s_instance;
   }

   void RenderUtils::DrawScreenQuad(GraphicsContext& context,
      ColorBuffer& rt, ColorBuffer& tex,
      uint16_t x, uint16_t y, uint16_t width, uint16_t height,
      const Vector2& uvLt, const Vector2& uvRb)
   {
      if (!s_instance->m_inited) {
         return;
      }

      GraphicsPSO pso = s_instance->m_quadPSO;
      pso.SetRenderTargetFormat(rt.GetFormat());
      pso.Finalize();

      context.SetRootSignature(s_instance->m_rootSignature);
      context.SetPipelineState(pso);
      context.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      context.SetRenderTarget(rt.GetRTV());
      context.SetViewportAndScissor(x, y, width, height);

      // todo: move outside this function
      context.SetDynamicDescriptor(0, 0, tex.GetSRV());

      struct Vertex
      {
         Vector2 pos;
         Vector2 uv;
      };

      CB_ALIGN Vertex vs[] = {
         { {-1, 1}, uvLt},
         { {-1, -1}, {uvLt.x, uvRb.y}},
         { {1, -1}, uvRb},
         { {1, 1}, {uvRb.x, uvLt.y}},
      };

      CB_ALIGN uint16_t inds[] = {
         0, 1, 2,
         2, 3, 0,
      };

      context.SetDynamicVB(0, _countof(vs), sizeof(Vertex), vs);
      context.SetDynamicIB(_countof(inds), inds);

      context.DrawIndexed(6);
   }

   void RenderUtils::DrawFullScreenQuad(GraphicsContext& context, ColorBuffer& rt, ColorBuffer& tex,
      const Vector2& uvLt, const Vector2& uvRb)
   {
      s_instance->DrawScreenQuad(context, rt, tex, 0, 0, rt.GetWidth(), rt.GetHeight(), uvLt, uvRb);
   }

   bool RenderUtils::BuildShadersAndPSO()
   {
      m_inited = false;

      using namespace Graphics;

      {
         auto vShader = Shader::Create("../Shaders/quad.hlsl", "quadVS", ShaderType::Vertex);
         auto pShader = Shader::Create("../Shaders/quad.hlsl", "quadPS", ShaderType::Pixel);
         if (vShader && pShader)
         {
            m_vertexShader = vShader;
            m_pixelShader = pShader;
         }
         else
         {
            return false;
         }
      }

      m_rootSignature.Reset(1, 1);
      m_rootSignature[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
      m_rootSignature.InitStaticSampler(0, SamplerLinearClampDesc);
      m_rootSignature.Finalize(L"Quad", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

      D3D12_INPUT_ELEMENT_DESC vertElem[] =
      {
          { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
          { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      };

      m_quadPSO.SetRootSignature(m_rootSignature);
      m_quadPSO.SetRasterizerState(RasterizerTwoSided);
      m_quadPSO.SetBlendState(BlendDisable);
      m_quadPSO.SetDepthStencilState(DepthStateDisabled);
      m_quadPSO.SetInputLayout(_countof(vertElem), vertElem);
      m_quadPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
      m_quadPSO.SetVertexShader(m_vertexShader->GetBytecode());
      m_quadPSO.SetPixelShader(m_pixelShader->GetBytecode());

      m_inited = true;
      return true;
   }
}

