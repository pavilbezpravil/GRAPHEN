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

      static ClothMeshRef Create(uint m, uint n);

      StructuredBuffer& GetPositionBuffer();
      StructuredBuffer& GetNormalBuffer();
      // StructuredBuffer& GetSimulatedTangentVB();

      StructuredBuffer& GetPositionBackBuffer();
      StructuredBuffer& GetNormalBackBuffer();
      // StructuredBuffer& GetSimulationTangentTargetVB();

      uint GetSimulatedBufferIdx() const;
      uint GetSimulationTargetBufferIdx() const;

      const void SetDrawBuffers(GraphicsContext& context) const override;
      const uint GetDrawIndexCount() const override;

      void SwapBuffers();
   private:
      MeshData m_meshData;

      const static uint BB_BUFFERS = 2;

      uint m_curBufferIdx;
      StructuredBuffer m_posBuffer[BB_BUFFERS];
      StructuredBuffer m_normalsBuffer[BB_BUFFERS];
      StructuredBuffer m_tangentBuffer[BB_BUFFERS];
      StructuredBuffer m_texBuffer;

      ByteAddressBuffer m_indexBuffer;
      uint m_m, m_n;

      void CreateGPUBuffers();
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
