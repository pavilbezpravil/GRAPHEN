#include "gnpch.h"

#include "Cloth.h"
#include "GeometryGenerator.h"


namespace gn {

   namespace {
      struct DistanceConstraint {
         uint p1Idx;
         uint p2Idx;
         float d;
      };

      enum class ClothRootSignature : uint {
         kPrevPosition = 0,
         kConstrains,
         kPosition,
         kVelocity,
         kNormal,
         kTmpPosition,
         kConstancBuffer,
         kCount,
      };

      struct cbComputePass {
         uint gNSize;
         uint gMSize;
         uint gNParticles;
         uint gNConstrains;
         float gTime;
         float gDeltaTime;
         float gRestDist;
         float gKVelocityDump;
         float gKs;
         float gKs_diagonal;
         float gKs_bend;
         float gUseDiagonal;
         float gUseBend;
         // Matrix4 gModel;
         // Matrix4 gModelInv;
         // Matrix4 gModelNormal;
         // Matrix4 gModelNormalInv;
      };
   }

   ClothMesh::ClothMesh(uint m, uint n, const Matrix4& meshTransform, const std::string& name)
                     : BaseMesh(name), m_meshTransform(meshTransform), m_isConstrainsDirty(false) {
      RebuildMesh(m, n);
   }

   ClothMeshRef ClothMesh::Create(uint m, uint n, const Matrix4& transform) {
      return CreateRef<ClothMesh>(m, n, transform);
   }

   void ClothMesh::RebuildMesh(uint m, uint n, bool force) {
      if (!force && m_m == m && m_n == n) {
         return;
      }

      m_m = m;
      m_n = n;

      Vector3 scale;
      Vector3 _translation;
      Quaternion _rot;

      m_meshTransform.Decompose(scale, _rot, _translation);
      m_width = scale.x;
      m_height = scale.z;

      m_meshData = GeometryGenerator::CreateGrid(1, 1, m_m, m_n);
      Matrix4 transformNormal = Transpose(Invert(m_meshTransform));
      for (Vertex& v : m_meshData.Vertices) {
         v.Position = v.Position * m_meshTransform;
         v.Normal = v.Normal * transformNormal;
      }

      CreateGPUBuffers();
   }

   StructuredBuffer& ClothMesh::GetPositionBuffer() {
      return m_posBuffer;
   }

   StructuredBuffer& ClothMesh::GetVelocityBuffer() {
      return m_velocityBuffer;
   }

   StructuredBuffer& ClothMesh::GetNormalBuffer() {
      return m_normalsBuffer;
   }

   StructuredBuffer& ClothMesh::GetPositionTmpBuffer(uint ind) {
      return m_posTmpBuffer[ind];
   }

   StructuredBuffer& ClothMesh::GetConstraintsBuffer()
   {
      return m_constraintsBuffer;
   }

   void ClothMesh::PrepareDrawBuffers(CommandContext& context) {
      context.BeginResourceTransition(m_posBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.BeginResourceTransition(m_normalsBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.BeginResourceTransition(m_tangentBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
      context.BeginResourceTransition(m_texBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
   }

   void ClothMesh::SetDrawBuffers(GraphicsContext& context) {
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

   const uint ClothMesh::GetDrawIndexCount() const {
      return m_indexBuffer.GetElementCount();
   }

   void ClothMesh::InitConstrainsBuffer(uint maxConstrains)
   {
      m_constraintsBuffer.Create(L"CLoth Constrains buffer", maxConstrains, sizeof(ClothConstraint::Constraint));
   }

   void ClothMesh::SetConstrains(const std::vector<ClothConstraint::Constraint>& constraints)
   {
      GN_ASSERT(constraints.size() <= m_constraintsBuffer.GetBufferSize());
      // todo: m_constraintsBufferSize constrains of more
      if (!constraints.empty() && constraints.size() <= m_constraintsBuffer.GetBufferSize())
      {
         m_constraints = constraints;
         m_isConstrainsDirty = true;
      }
   }

   const std::vector<ClothConstraint::Constraint>& ClothMesh::GetConstrains() const
   {
      return m_constraints;
   }

   void ClothMesh::CreateGPUBuffers() {
      MeshUtils::BuildSeparateBuffersForVertex(m_meshData.Vertices, &m_posBuffer, &m_normalsBuffer,
                                               &m_tangentBuffer, &m_texBuffer);

      std::vector<Vector3> velocity;
      velocity.resize(m_meshData.Vertices.size());
      ZeroMemory(velocity.data(), velocity.size() * sizeof(Vector3));
      m_velocityBuffer.Create(L"Cloth velocity buffer", (uint32)m_meshData.Vertices.size(), sizeof(Vector3), velocity.data());

      m_posTmpBuffer[0].Create(L"Cloth pos tmp 0 buffer", (uint32)m_meshData.Vertices.size(), sizeof(Vector3), nullptr);
      m_posTmpBuffer[1].Create(L"Cloth pos tmp 1 buffer", (uint32)m_meshData.Vertices.size(), sizeof(Vector3), nullptr);

      // float restDistanceX = 0.1f;
      // float restDistanceY = 0.1f;
      //
      // std::vector<DistanceConstraint> constrains;
      //
      // for (int i = 0; i < m_n; ++i) {
      //    for (int j = 0; j < m_n; ++j) {
      //       auto addConstaint = [&](int iNeight, int jNeight, float restDist) {
      //          if (iNeight >= 0 && iNeight < m_n && jNeight >= 0 && jNeight < m_m) {
      //             constrains.push_back({ j * m_n + i, jNeight * m_n + iNeight, restDist });
      //          }
      //       };
      //
      //       if (i % 2 == 0) {
      //          addConstaint(i, j + 1, restDistanceY);
      //       } else {
      //          addConstaint(i - 1, j, restDistanceX);
      //          addConstaint(i, j + 1, restDistanceY);
      //          addConstaint(i + 1, j, restDistanceX);
      //       }
      //    }
      // }
      // m_constraintsBuffer.Create(L"Cloth constrains buffer", constrains);

      m_indexBuffer.Create(L"Indexes", (uint32)m_meshData.Indices32.size(), sizeof(uint32), m_meshData.Indices32.data());
   }

   ClothSimulation::ClothSimulation() : m_inited(false) {
      m_solvePass = true;
      m_deltaRimeMultiplier = 1.;
   }

   bool ClothSimulation::Init() {
      m_cbComputePassBuffer.Create(L"Cloth constant buffer", 1, sizeof(cbComputePass));
      return RebuildShaderAndPSO();
   }

   bool ClothSimulation::RebuildShaderAndPSO() {
      m_inited = CreateShaders() && CreatePSO();
      return m_inited;
   }

   void ClothSimulation::AddSimCloth(ClothMeshRef& cloth) {
      m_simClothes.push_back(cloth);
   }

   void ClothSimulation::Update(ComputeContext& context, Timestep ts) {
      if (!m_psoPrepare && !m_psoSolve && !m_psoRecord && !m_psoComputeNormal) {
         return;
      }

      float deltaTime = ts.GetSeconds() * m_deltaRimeMultiplier;

      static float simTime = 0;
      simTime += deltaTime;

      auto makeCB = [&] (ClothMesh& cloth) {
         CB_ALIGN cbComputePass cPass;
         cPass.gNSize = cloth.GetN();
         cPass.gMSize = cloth.GetM();
         cPass.gNParticles = cloth.GetN() * cloth.GetM();
         // cPass.gNConstrains = m_constraints.size();
         cPass.gTime = simTime;
         cPass.gDeltaTime = deltaTime;
         cPass.gRestDist = cloth.GetWidth() / float(cloth.GetN() - 1);
         cPass.gKVelocityDump = gKVelocityDump;
         cPass.gKs = gKs;
         cPass.gKs_diagonal = gKs_diagonal;
         cPass.gKs_bend = gKs_bend;
         cPass.gUseDiagonal = gUseDiagonal;
         cPass.gUseBend = gUseBend;
         // cPass.gModel = toWorld;
         // cPass.gModelInv = Invert(toWorld);
         // cPass.gModelNormal = Transpose(cPass.gModelInv);
         // cPass.gModelNormalInv = Invert(cPass.gModelNormal);
         return cPass;
      };

      // todo:
      CB_ALIGN cbComputePass cPass = makeCB(*m_simClothes[0]);

      context.WriteBuffer(m_cbComputePassBuffer, 0, &cPass, sizeof(cPass));
      context.TransitionResource(m_cbComputePassBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);

      {
         GPU_EVENT_SCOPE("Constrains copy");
         for (auto& pCloth : m_simClothes) {
            ClothMesh& cloth = *pCloth;

            if (cloth.m_isConstrainsDirty && !cloth.m_constraints.empty()) {
               context.WriteBuffer(cloth.m_constraintsBuffer, 0, cloth.m_constraints.data(), sizeof(ClothConstraint::Constraint) * cloth.m_constraints.size());
               cloth.m_isConstrainsDirty = false;
            }
         }
      }

      {
         GPU_EVENT_SCOPE("Insert Barriers");
         for (auto& pCloth : m_simClothes) {
            ClothMesh& cloth = *pCloth;

            context.TransitionResource(cloth.GetPositionBuffer(), D3D12_RESOURCE_STATE_GENERIC_READ);
            context.TransitionResource(cloth.GetPositionTmpBuffer(0), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            context.TransitionResource(cloth.GetPositionTmpBuffer(1), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            context.TransitionResource(cloth.GetVelocityBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            context.TransitionResource(cloth.GetNormalBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            context.TransitionResource(cloth.GetConstraintsBuffer(), D3D12_RESOURCE_STATE_GENERIC_READ);
            context.InsertUAVBarrier(cloth.GetPositionTmpBuffer(0));
            context.InsertUAVBarrier(cloth.GetPositionTmpBuffer(1));
            context.InsertUAVBarrier(cloth.GetVelocityBuffer());
            context.InsertUAVBarrier(cloth.GetNormalBuffer());
         }
         context.FlushResourceBarriers();
      }

      context.SetRootSignature(m_rootSignature);

      const uint DISPATCH_SIZE = 8;
      uint posInd = 0;

      {
         GPU_EVENT_SCOPE("Cloth prepare");

         {
            GPU_EVENT_SCOPE("Insert UAV barriers");
            for (auto& pCloth : m_simClothes) {
               ClothMesh& cloth = *pCloth;
               context.InsertUAVBarrier(cloth.GetPositionTmpBuffer(posInd));
               context.InsertUAVBarrier(cloth.GetPositionBuffer());
               context.InsertUAVBarrier(cloth.GetVelocityBuffer());
            }
            context.FlushResourceBarriers();
         }

         for (auto& pCloth : m_simClothes) {
            ClothMesh& cloth = *pCloth;

            if (m_solvePass) {
               auto& pos = cloth.GetPositionTmpBuffer(posInd);

               // context.InsertUAVBarrier(pos);
               // context.InsertUAVBarrier(cloth.GetPositionBuffer());
               // context.InsertUAVBarrier(cloth.GetVelocityBuffer(), true);

               context.SetPipelineState(m_psoPrepare);
               context.SetBufferSRV((uint)ClothRootSignature::kPrevPosition, cloth.GetPositionBuffer());
               context.SetBufferUAV((uint)ClothRootSignature::kPosition, pos);
               context.SetBufferUAV((uint)ClothRootSignature::kVelocity, cloth.GetVelocityBuffer());

               // context.SetDynamicConstantBufferView((uint)ClothRootSignature::kConstancBuffer, sizeof(cbComputePass), &cPass);
               context.SetConstantBuffer((uint)ClothRootSignature::kConstancBuffer, m_cbComputePassBuffer.GetGpuVirtualAddress());

               context.Dispatch2D(cloth.GetN(), cloth.GetM(), DISPATCH_SIZE, DISPATCH_SIZE);
            } else {
               context.CopyBuffer(cloth.GetPositionTmpBuffer(posInd), cloth.GetPositionBuffer());
               context.TransitionResource(cloth.GetPositionTmpBuffer(posInd), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
               context.TransitionResource(cloth.GetPositionBuffer(), D3D12_RESOURCE_STATE_GENERIC_READ, true);
            }
         }
      }

      {
         if (m_solvePass) {
            GPU_EVENT_SCOPE("Solve Iters");
            for (int i = 0; i < m_iter; ++i) {
               GPU_EVENT_SCOPE("Solve Iter");
               {
                  GPU_EVENT_SCOPE("Insert UAV barriers");
                  for (auto& pCloth : m_simClothes) {
                     ClothMesh& cloth = *pCloth;
                     context.InsertUAVBarrier(cloth.GetPositionTmpBuffer(posInd));
                     context.InsertUAVBarrier(cloth.GetPositionTmpBuffer(1 - posInd));
                  }
                  context.FlushResourceBarriers();
               }

               for (auto& pCloth : m_simClothes) {
                  ClothMesh& cloth = *pCloth;
                  auto& pos = cloth.GetPositionTmpBuffer(posInd);
                  auto& posTmp = cloth.GetPositionTmpBuffer(1 - posInd);

                  // context.InsertUAVBarrier(pos);
                  // context.InsertUAVBarrier(posTmp, true);

                  context.SetPipelineState(m_psoSolve);
                  context.SetBufferSRV((uint)ClothRootSignature::kConstrains, cloth.GetConstraintsBuffer()); // todo: add size to shader
                  context.SetBufferUAV((uint)ClothRootSignature::kPosition, pos);
                  context.SetBufferUAV((uint)ClothRootSignature::kTmpPosition, posTmp);

                  // context.SetDynamicConstantBufferView((uint)ClothRootSignature::kConstancBuffer, sizeof(cbComputePass), &cPass);
                  context.SetConstantBuffer((uint)ClothRootSignature::kConstancBuffer, m_cbComputePassBuffer.GetGpuVirtualAddress());

                  context.Dispatch2D(cloth.GetN(), cloth.GetM(), DISPATCH_SIZE, DISPATCH_SIZE);
               }

               posInd = 1 - posInd;
            }
         }
      }

      {
         GPU_EVENT_SCOPE("Record");

         {
            GPU_EVENT_SCOPE("Insert UAV barriers");
            for (auto& pCloth : m_simClothes) {
               ClothMesh& cloth = *pCloth;
               context.InsertUAVBarrier(cloth.GetVelocityBuffer());
               context.InsertUAVBarrier(cloth.GetPositionTmpBuffer(posInd));
               context.InsertUAVBarrier(cloth.GetPositionTmpBuffer(1 - posInd));
            }
            context.FlushResourceBarriers();
         }

         for (auto& pCloth : m_simClothes) {
            ClothMesh& cloth = *pCloth;
            auto& pos = cloth.GetPositionTmpBuffer(posInd);
            auto& posTmp = cloth.GetPositionTmpBuffer(1 - posInd);

            // context.InsertUAVBarrier(cloth.GetVelocityBuffer());
            // context.InsertUAVBarrier(pos);
            // context.InsertUAVBarrier(posTmp, true);

            context.SetPipelineState(m_psoRecord);
            context.SetBufferSRV((uint)ClothRootSignature::kPrevPosition, cloth.GetPositionBuffer());
            context.SetBufferUAV((uint)ClothRootSignature::kPosition, pos);
            context.SetBufferUAV((uint)ClothRootSignature::kVelocity, cloth.GetVelocityBuffer());
            context.SetBufferUAV((uint)ClothRootSignature::kTmpPosition, posTmp);

            // context.SetDynamicConstantBufferView((uint)ClothRootSignature::kConstancBuffer, sizeof(cbComputePass), &cPass);
            context.SetConstantBuffer((uint)ClothRootSignature::kConstancBuffer, m_cbComputePassBuffer.GetGpuVirtualAddress());

            context.Dispatch2D(cloth.GetN(), cloth.GetM(), DISPATCH_SIZE, DISPATCH_SIZE);
         }

         posInd = 1 - posInd;
      }

      {
         GPU_EVENT_SCOPE("Compute normal");

         {
            GPU_EVENT_SCOPE("Insert UAV barriers");
            for (auto& pCloth : m_simClothes) {
               ClothMesh& cloth = *pCloth;
               context.InsertUAVBarrier(cloth.GetPositionTmpBuffer(posInd));
            }
            context.FlushResourceBarriers();
         }

         for (auto& pCloth : m_simClothes) {
            ClothMesh& cloth = *pCloth;

            auto& pos = cloth.GetPositionTmpBuffer(posInd);
            auto& normal = cloth.GetNormalBuffer();

            // context.TransitionResource(normal, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            // context.InsertUAVBarrier(normal);
            // context.InsertUAVBarrier(pos);
            // context.InsertUAVBarrier(normal, true);

            context.SetPipelineState(m_psoComputeNormal);
            context.SetBufferUAV((uint)ClothRootSignature::kPosition, pos);
            context.SetBufferUAV((uint)ClothRootSignature::kNormal, normal);

            // context.SetDynamicConstantBufferView((uint)ClothRootSignature::kConstancBuffer, sizeof(cbComputePass), &cPass);
            context.SetConstantBuffer((uint)ClothRootSignature::kConstancBuffer, m_cbComputePassBuffer.GetGpuVirtualAddress());

            context.Dispatch2D(cloth.GetN(), cloth.GetM(), DISPATCH_SIZE, DISPATCH_SIZE);

            context.BeginResourceTransition(normal, D3D12_RESOURCE_STATE_GENERIC_READ);
         }
      }

      {
         GPU_EVENT_SCOPE("Copy pos");
         {
            GPU_EVENT_SCOPE("Insert UAV barriers");
            for (auto& pCloth : m_simClothes) {
               ClothMesh& cloth = *pCloth;
               context.TransitionResource(cloth.GetPositionBuffer(), D3D12_RESOURCE_STATE_COPY_DEST);
               context.TransitionResource(cloth.GetPositionTmpBuffer(posInd), D3D12_RESOURCE_STATE_COPY_SOURCE);
            }
            context.FlushResourceBarriers();
         }

         for (auto& pCloth : m_simClothes) {
            ClothMesh& cloth = *pCloth;
            auto& curPos = cloth.GetPositionBuffer();
            context.CopyBuffer(curPos, cloth.GetPositionTmpBuffer(posInd));
            // context.BeginResourceTransition(curPos, D3D12_RESOURCE_STATE_GENERIC_READ);
            // context.BeginResourceTransition(cloth.GetPositionTmpBuffer(posInd), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
         }

         {
            GPU_EVENT_SCOPE("Insert UAV barriers");
            for (auto& pCloth : m_simClothes) {
               ClothMesh& cloth = *pCloth;
               context.BeginResourceTransition(cloth.GetPositionBuffer(), D3D12_RESOURCE_STATE_GENERIC_READ);
               context.BeginResourceTransition(cloth.GetPositionTmpBuffer(posInd), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            }
            context.FlushResourceBarriers();
         }
      }
   }

   bool ClothSimulation::CreateShaders() {
      // auto cShader = Shader::Create("../Shaders/cloth.hlsl", "clothCS", ShaderType::Compute);
      // if (!cShader) {
      //    return false;
      // }
      // m_cShader = std::move(cShader);
      return true;
   }

   bool ClothSimulation::CreatePSO() {
      m_rootSignature.Reset((uint)ClothRootSignature::kCount, 0);
      m_rootSignature[(uint)ClothRootSignature::kPrevPosition].InitAsBufferSRV(0);
      m_rootSignature[(uint)ClothRootSignature::kConstrains].InitAsBufferSRV(1);
      m_rootSignature[(uint)ClothRootSignature::kPosition].InitAsBufferUAV(0);
      m_rootSignature[(uint)ClothRootSignature::kVelocity].InitAsBufferUAV(1);
      m_rootSignature[(uint)ClothRootSignature::kNormal].InitAsBufferUAV(2);
      m_rootSignature[(uint)ClothRootSignature::kTmpPosition].InitAsBufferUAV(3);
      m_rootSignature[(uint)ClothRootSignature::kConstancBuffer].InitAsConstantBuffer(0);
      m_rootSignature.Finalize(L"Cloth simulation");

      {
         auto cShader = Shader::Create("../Shaders/cloth.hlsl", "applyForcesDumpVelocityAssignPositionCS", ShaderType::Compute);

         if (cShader) {
            auto& pso = m_psoPrepare;
            pso.SetRootSignature(m_rootSignature);
            pso.SetComputeShader(cShader->GetBytecode());
            pso.Finalize();
         }
      }

      {
         auto cShader = Shader::Create("../Shaders/cloth.hlsl", "solveCS", ShaderType::Compute);

         if (cShader) {
            auto& pso = m_psoSolve;
            pso.SetRootSignature(m_rootSignature);
            pso.SetComputeShader(cShader->GetBytecode());
            pso.Finalize();
         }
      }

      {
         auto cShader = Shader::Create("../Shaders/cloth.hlsl", "recordCS", ShaderType::Compute);

         if (cShader) {
            auto& pso = m_psoRecord;
            pso.SetRootSignature(m_rootSignature);
            pso.SetComputeShader(cShader->GetBytecode());
            pso.Finalize();
         }
      }

      {
         auto cShader = Shader::Create("../Shaders/cloth.hlsl", "computeNormalCS", ShaderType::Compute);

         if (cShader) {
            auto& pso = m_psoComputeNormal;
            pso.SetRootSignature(m_rootSignature);
            pso.SetComputeShader(cShader->GetBytecode());
            pso.Finalize();
         }
      }

      return true;
   }
}
