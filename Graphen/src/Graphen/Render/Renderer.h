#pragma once

#include "ColorBuffer.h"
#include "DepthBuffer.h"

namespace gn
{
   class Renderer
   {
   public:
      Renderer();

      bool Init(uint16_t width, uint16_t height);
      void Destroy();
      bool Resize(uint16_t width, uint16_t height);

      uint16_t GetWidth() const { return m_width; }
      uint16_t GetHeight() const { return m_height; }

      DXGI_FORMAT GetColorLDRFormat() const { return m_colorFormat; }
      DXGI_FORMAT GetDepthFormat() const { return m_depthFormat; }

      ColorBuffer& GetColorBufferLDR();
      DepthBuffer& GetDepth();

      void Present();

   private:
      uint16_t m_width;
      uint16_t m_height;

      DXGI_FORMAT m_colorFormat;
      ColorBuffer m_colorBufferLDR;

      DXGI_FORMAT m_depthFormat;
      DepthBuffer m_depth;
   };
}
