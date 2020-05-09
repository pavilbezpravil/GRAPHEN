#include "gnpch.h"
#include "BaseMesh.h"


namespace gn {
   const StructuredBuffer& Mesh::GetVertexBufferForDraw() const {
      return m_VertexBuffer;
   }

   const ByteAddressBuffer& Mesh::GetIndexBufferForDraw() const {
      return m_IndexBuffer;
   }
}

