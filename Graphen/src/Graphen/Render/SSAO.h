#pragma once

#include "gnpch.h"
#include "ColorBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "Shader.h"


namespace gn {
   class SSAO {
   public:
      SSAO();

      bool Init(uint width, uint height, DXGI_FORMAT format);
      bool Resize(uint width, uint height);

      void ComputeSSAO(GraphicsContext& context, ColorBuffer& normalBuffer);

   private:
      ColorBuffer m_texAO;
      DXGI_FORMAT m_aoFormat;

      RootSignature m_rootSignature;
      GraphicsPSO m_pso;

      ShaderRef m_vShader;
      ShaderRef m_pShader;
   };

}
