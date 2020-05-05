#pragma once

#include "ColorBuffer.h"
#include "DepthBuffer.h"


namespace Math {
   class Camera;
}


namespace gn
{
   class Scene;


   class Renderer
   {
   public:
      Renderer();

      bool Init(uint16_t width, uint16_t height);
      void Destroy();
      bool Resize(uint16_t width, uint16_t height);

      void DrawScene(const Scene& scene, const Camera& camera);

      uint16_t GetWidth() const { return m_width; }
      uint16_t GetHeight() const { return m_height; }

      DXGI_FORMAT GetLDRFormat() const { return m_colorFormat; }
      DXGI_FORMAT GetDepthFormat() const { return m_depthFormat; }

      ColorBuffer& GetLDRTarget();
      ColorBuffer& GetLDRBB();
      DepthBuffer& GetDepth();

      void SwapLDRBuffer();
      void Present();

      void ResetLDRIndex();
      uint8 GetBBIndex() const;
      uint8 GetTargetIndex() const;

   private:
      uint16_t m_width;
      uint16_t m_height;

      DXGI_FORMAT m_colorFormat;
      ColorBuffer m_colorBufferLDR[2];
      uint8 m_curLDRTarget;

      DXGI_FORMAT m_depthFormat;
      DepthBuffer m_depth;
   };
}
