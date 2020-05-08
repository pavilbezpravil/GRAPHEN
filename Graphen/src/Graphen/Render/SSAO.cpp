#include "gnpch.h"

#include "SSAO.h"
#include "GraphicsCommon.h"

using namespace Graphics;

namespace gn {
   SSAO::SSAO() {}

   bool SSAO::Init(uint width, uint height, DXGI_FORMAT format) {
      // m_aoFormat = format;
      //
      // {
      //    auto vShader = Shader::Create("../Shaders/ao.hlsl", "aoVS", ShaderType::Vertex);
      //    auto pShader = Shader::Create("../Shaders/ao.hlsl", "aoPS", ShaderType::Pixel);
      //    if (vShader && pShader) {
      //       m_vShader = vShader;
      //       m_pShader = pShader;
      //    } else {
      //       return false;
      //    }
      // }
      //
      // m_rootSignature.Reset(1, 0);
      // m_rootSignature[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
      // m_rootSignature.Finalize(L"AO RootSignature");
      //
      // D3D12_INPUT_ELEMENT_DESC vertElem[] =
      // {
      //     { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      // };
      //
      // m_pso.SetRootSignature(m_rootSignature);
      // m_pso.SetRasterizerState(RasterizerDefault);
      // m_pso.SetBlendState(BlendDisable);
      // m_pso.SetDepthStencilState(DepthStateDisabled);
      // m_pso.SetInputLayout(_countof(vertElem), vertElem);
      // m_pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
      // m_pso.SetRenderTargetFormat(m_aoFormat);
      // m_pso.SetVertexShader(m_vShader->GetBytecode());
      // m_pso.SetPixelShader(m_pShader->GetBytecode());
      // m_pso.Finalize();
      //
      // return Resize(width, height);
      return false;
   }

   bool SSAO::Resize(uint width, uint height) {
      m_texAO.Create(L"AO Buffer", width, height, 1, m_aoFormat);
      return true;
   }

}
