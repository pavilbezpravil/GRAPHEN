#include "gnpch.h"

#include "Cloth.h"
#include "GeometryGenerator.h"
#include "VidDriver.h"


namespace gn {
   ClothMesh::ClothMesh(const MeshData& meshData, uint m, uint n, const std::string& name)
                     : BaseMesh(name), m_meshData(meshData), m_m(m), m_n(n), m_curBufferIdx(0) {
      CreateGPUBuffers();
   }

   ClothMeshRef ClothMesh::Create(uint m, uint n) {
      MeshData meshData = GeometryGenerator::CreateGrid(1, 1, m, n);
      return CreateRef<ClothMesh>(meshData, m, n);
   }

   StructuredBuffer& ClothMesh::GetPositionBuffer() {
      return m_posBuffer[m_curBufferIdx];
   }

   StructuredBuffer& ClothMesh::GetNormalBuffer() {
      return m_normalsBuffer[m_curBufferIdx];
   }

   StructuredBuffer& ClothMesh::GetPositionBackBuffer() {
      return m_posBuffer[GetSimulationTargetBufferIdx()];
   }

   StructuredBuffer& ClothMesh::GetNormalBackBuffer() {
      return m_normalsBuffer[GetSimulationTargetBufferIdx()];
   }

   uint ClothMesh::GetSimulatedBufferIdx() const {
      return m_curBufferIdx;
   }

   uint ClothMesh::GetSimulationTargetBufferIdx() const {
      return 1 - m_curBufferIdx;
   }

   const void ClothMesh::SetDrawBuffers(GraphicsContext& context) const {
      const D3D12_VERTEX_BUFFER_VIEW VBViews[] = {
         m_posBuffer[GetSimulatedBufferIdx()].VertexBufferView(), m_normalsBuffer[GetSimulatedBufferIdx()].VertexBufferView(),
         m_tangentBuffer[GetSimulatedBufferIdx()].VertexBufferView(), m_texBuffer.VertexBufferView(),
      };
      context.SetVertexBuffers(0, _countof(VBViews), VBViews);
      context.SetIndexBuffer(m_indexBuffer.IndexBufferView());
   }

   const uint ClothMesh::GetDrawIndexCount() const {
      return m_indexBuffer.GetElementCount();
   }

   void ClothMesh::SwapBuffers() {
      m_curBufferIdx = GetSimulationTargetBufferIdx();
   }

   void ClothMesh::CreateGPUBuffers() {
      MeshUtils::BuildSeparateBuffersForVertex(m_meshData.Vertices, &m_posBuffer[0], &m_normalsBuffer[0],
                                               &m_tangentBuffer[0], &m_texBuffer);
      m_posBuffer[1].Create(L"Cloth pos additional buffer", (uint32)m_meshData.Vertices.size(), sizeof(Vector3),nullptr);
      m_normalsBuffer[1].Create(L"Cloth normal additional buffer", (uint32)m_meshData.Vertices.size(), sizeof(Vector3), nullptr);

      // todo: tmp, because normals is not computed now
      GraphicsContext& context = GraphicsContext::Begin(L"Copy cloth normal");
      context.CopyBuffer(m_normalsBuffer[1], m_normalsBuffer[0]);
      context.Finish();

      m_tangentBuffer[1].Create(L"Cloth tangent additional buffer", (uint32)m_meshData.Vertices.size(), sizeof(Vector3),nullptr);

      m_indexBuffer.Create(L"Indexes", (uint32)m_meshData.Indices32.size(), sizeof(uint32), m_meshData.Indices32.data());
   }

   ClothSimulation::ClothSimulation() : m_inited(false) {}

   bool ClothSimulation::Init() {
      m_inited = CreateShaders() && CreatePSO();
      return m_inited;
   }

   void ClothSimulation::Update(ComputeContext& context, ClothMesh& cloth, const Matrix4& toWorld, Timestep ts) {
      context.TransitionResource(cloth.GetPositionBackBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
      context.TransitionResource(cloth.GetNormalBackBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

      context.SetPipelineState(m_pso);
      context.SetRootSignature(m_rootSignature);
      context.SetBufferSRV(0, cloth.GetPositionBuffer());
      context.SetBufferUAV(1, cloth.GetPositionBackBuffer());

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

      // todo: begin transition
      context.TransitionResource(cloth.GetPositionBackBuffer(), D3D12_RESOURCE_STATE_GENERIC_READ);
      context.TransitionResource(cloth.GetNormalBackBuffer(), D3D12_RESOURCE_STATE_GENERIC_READ);

      cloth.SwapBuffers();

      Graphics::g_CommandManager.IdleGPU();
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
