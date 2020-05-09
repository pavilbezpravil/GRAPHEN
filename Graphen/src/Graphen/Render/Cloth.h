#pragma once

#include "gnpch.h"
#include "Graphen/Core/Timestep.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "Shader.h"
#include "Scene.h"


namespace gn {

   class ClothMesh;
   using ClothMeshRef = sptr<ClothMesh>;

   class ClothMesh : public BaseMesh {
   public:
      ClothMesh(const MeshData& meshData, uint m, uint n, const std::string& name = "");

      const StructuredBuffer& GetVertexBufferForDraw() const override;
      const ByteAddressBuffer& GetIndexBufferForDraw() const override;

      static ClothMeshRef Create(uint m, uint n);

      StructuredBuffer& GetSimulatedVB();
      StructuredBuffer& GetSimulationTargetVB();

      uint GetSimulatedVBIdx() const;
      uint GetSimulationTargetVBIdx() const;

      void SwapVB();
   private:
      MeshData m_meshData;

      uint m_simulatedVB;
      StructuredBuffer m_vertexBuffer[3];
      ByteAddressBuffer m_indexBuffer;

      void CreateGPUBuffers() {
         m_vertexBuffer[0].Create(L"Cloth vertex 0", (uint32)m_meshData.Vertices.size(), sizeof(Vertex), m_meshData.Vertices.data());
         m_vertexBuffer[1].Create(L"Cloth vertex 1", (uint32)m_meshData.Vertices.size(), sizeof(Vertex), nullptr);
         m_vertexBuffer[2].Create(L"Cloth vertex 2", (uint32)m_meshData.Vertices.size(), sizeof(Vertex), nullptr);
         m_indexBuffer.Create(L"Indexes", (uint32)m_meshData.Indices32.size(), sizeof(uint32), m_meshData.Indices32.data());
      }

      uint m_m, m_n;
   };
   

   class ClothSimulation {
   public:
      ClothSimulation();
      bool Init();

      void Update(ComputeContext& context, ClothMesh& cloth, const Matrix4& toWorld, Timestep ts);

   private:
      bool m_inited;
      RootSignature m_rootSignature;
      ComputePSO m_pso;

      ShaderRef m_cShader;

      bool CreateShaders();
      bool CreatePSO();
   };

}
