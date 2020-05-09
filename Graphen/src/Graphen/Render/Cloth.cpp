#include "gnpch.h"

#include "Cloth.h"
#include "GeometryGenerator.h"
#include "VidDriver.h"


namespace gn {
   ClothMesh::ClothMesh(const MeshData& meshData, uint m, uint n, const std::string& name)
                     : BaseMesh(name), m_meshData(meshData), m_m(m), m_n(n), m_simulatedVB(0) {
      CreateGPUBuffers();
   }

   const StructuredBuffer& ClothMesh::GetVertexBufferForDraw() const {
      return m_vertexBuffer[m_simulatedVB];
   }
   const ByteAddressBuffer& ClothMesh::GetIndexBufferForDraw() const {
      return m_indexBuffer;
   }

   ClothMeshRef ClothMesh::Create(uint m, uint n) {
      MeshData meshData = GeometryGenerator::CreateGrid(1, 1, m, n);
      return CreateRef<ClothMesh>(meshData, m, n);
   }

   StructuredBuffer& ClothMesh::GetSimulatedVB() {
      return m_vertexBuffer[m_simulatedVB];
   }

   StructuredBuffer& ClothMesh::GetSimulationTargetVB() {
      return m_vertexBuffer[GetSimulationTargetVBIdx()];
   }

   uint ClothMesh::GetSimulatedVBIdx() const {
      return m_simulatedVB;
   }

   uint ClothMesh::GetSimulationTargetVBIdx() const {
      return (m_simulatedVB + 1) % 3;
   }

   void ClothMesh::SwapVB() {
      m_simulatedVB = GetSimulationTargetVBIdx();
   }

   ClothSimulation::ClothSimulation() : m_inited(false) {}

   bool ClothSimulation::Init() {
      m_inited = CreateShaders() && CreatePSO();
      return m_inited;
   }

   void ClothSimulation::Update(ComputeContext& context, ClothMesh& cloth, const Matrix4& toWorld, Timestep ts) {
      context.TransitionResource(cloth.GetSimulatedVB(), D3D12_RESOURCE_STATE_GENERIC_READ);
      context.TransitionResource(cloth.GetSimulationTargetVB(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

      context.SetPipelineState(m_pso);
      context.SetRootSignature(m_rootSignature);
      context.SetBufferSRV(0, cloth.GetSimulatedVB());
      context.SetBufferUAV(1, cloth.GetSimulationTargetVB());

      CB_ALIGN struct cbComputePass {
         float gTime;
         float pad0;
         float pad1;
         float pad2;
         float gDeltaTime;
      };

      static float simTime = 0;
      simTime += ts.GetSeconds();
      cbComputePass cPass;
      cPass.gTime = simTime;
      cPass.gDeltaTime = ts.GetSeconds();

      context.SetDynamicConstantBufferView(2, sizeof(cbComputePass), &cPass);
      context.Dispatch2D(1, 1, 32 * 32, 1); // todo: hard code values

      context.TransitionResource(cloth.GetSimulationTargetVB(), D3D12_RESOURCE_STATE_GENERIC_READ);
   }

   bool ClothSimulation::CreateShaders() {
      auto cShader = Shader::Create("../Shaders/cloth.hlsl", "clothCS", ShaderType::Compute);
      if (!cShader) {
         return false;
      }
      m_cShader = std::move(cShader);
      return true;
   }

   bool ClothSimulation::CreatePSO() {
      if (!m_cShader) {
         return  false;
      }

      m_rootSignature.Reset(3, 0);
      m_rootSignature[0].InitAsBufferSRV(0);
      m_rootSignature[1].InitAsBufferUAV(0);
      m_rootSignature[2].InitAsConstantBuffer(0);
      m_rootSignature.Finalize(L"Cloth simulation");

      ComputePSO pso;
      pso.SetRootSignature(m_rootSignature);
      pso.SetComputeShader(m_cShader->GetBytecode());
      pso.Finalize();

      m_pso = pso;
      return true;
   }
}
