#include "gnpch.h"
#include "BaseMesh.h"


namespace gn {
   void MeshUtils::BuildSeparateBuffersForVertex(const std::vector<Vertex>& vs,
               StructuredBuffer* posBuffer, StructuredBuffer* normalBuffer,
               StructuredBuffer* tangentBuffer, StructuredBuffer* texBuffer) {
      std::vector<Vector3> pos(posBuffer ? vs.size() : 0);
      std::vector<Vector3> normal(normalBuffer ? vs.size() : 0);
      std::vector<Vector3> tangent(tangentBuffer ? vs.size() : 0);
      std::vector<Vector2> tex(texBuffer ? vs.size() : 0);

      for (int i = 0; i < vs.size(); ++i) {
         const Vertex& v = vs[i];
         if (posBuffer) {
            pos[i] = v.Position;
         }
         if (normalBuffer) {
            normal[i] = v.Normal;
         }
         if (tangentBuffer) {
            tangent[i] = v.TangentU;
         }
         if (texBuffer) {
            tex[i] = v.TexC;
         }
      }

      if (posBuffer) {
         posBuffer->Create(L"Position Buffer", vs.size(), sizeof(Vector3), pos.data());
      }
      if (normalBuffer) {
         normalBuffer->Create(L"Normal Buffer", vs.size(), sizeof(Vector3), normal.data());
      }
      if (tangentBuffer) {
         tangentBuffer->Create(L"Tangent Buffer", vs.size(), sizeof(Vector3), tangent.data());
      }
      if (texBuffer) {
         texBuffer->Create(L"TexC Buffer", vs.size(), sizeof(Vector2), tex.data());
      }
   }

   void StaticMesh::PrepareDrawBuffers(CommandContext& context) {
      context.BeginResourceTransition(m_posBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.BeginResourceTransition(m_normalsBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.BeginResourceTransition(m_tangentBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.BeginResourceTransition(m_texBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
   }

   void StaticMesh::SetDrawBuffers(GraphicsContext& context) {
      const D3D12_VERTEX_BUFFER_VIEW VBViews[] = {
         m_posBuffer.VertexBufferView(), m_normalsBuffer.VertexBufferView(),
         m_tangentBuffer.VertexBufferView(), m_texBuffer.VertexBufferView(),
      };

      context.TransitionResource(m_posBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.TransitionResource(m_normalsBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.TransitionResource(m_tangentBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.TransitionResource(m_texBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, true);

      context.SetVertexBuffers(0, _countof(VBViews), VBViews);
      context.SetIndexBuffer(m_indexBuffer.IndexBufferView());
   }

   const uint StaticMesh::GetDrawIndexCount() const {
      return m_indexBuffer.GetElementCount();
   }
}

