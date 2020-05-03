#pragma once

#include "Graphen/Math/VectorMath.h"
#include "Graphen/Render/CommandContext.h"
#include "Shader.h"


namespace gn
{
   class RenderUtils
   {
   public:
      RenderUtils();

      static bool Initialize();
      static void Shutdown();

      // void DrawQuad(GraphicsContext& context,
      //    PSO& pso,
      //    const Vector2& lt, const Vector2& rb,
      //    const Vector2& uvLt, const Vector2& uvRb);

      static void DrawScreenQuad(GraphicsContext& context,
         ColorBuffer& rt, ColorBuffer& tex,
         uint16_t x, uint16_t y, uint16_t width, uint16_t height,
         const Vector2& uvLt = Vector2::Zero, const Vector2& uvRb = Vector2::One);

      static void DrawFullScreenQuad(GraphicsContext& context,
         ColorBuffer& rt, ColorBuffer& tex,
         const Vector2& uvLt = Vector2::Zero, const Vector2& uvRb = Vector2::One);

   private:
      bool BuildShadersAndPSO();

      RootSignature m_rootSignature;
      GraphicsPSO m_quadPSO;

      sptr<Shader> m_vertexShader;
      sptr<Shader> m_pixelShader;

      bool m_inited;

      static RenderUtils* s_instance;
   };
}
