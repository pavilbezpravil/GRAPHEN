#pragma once

#include "gnpch.h"
#include "Graphen/Core/Timestep.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "Shader.h"
#include "Scene.h"


namespace gn {

   namespace ClothConstraint {
      enum class ConstraintType : uint {
         Attach = 1,
         SphereCollision,
      };

      struct Constraint {
         ConstraintType type;
         Vector3 data0;
         float data1;

         Constraint(ConstraintType type, const Vector3& data0, float data1) : type(type), data0(data0), data1(data1) {}
      };

      struct AttachConstraint : Constraint {
         AttachConstraint(const Vector3& pos) : Constraint(ConstraintType::Attach, pos, 0) {}
      };

      struct SphereCollisionConstraint : Constraint {
         SphereCollisionConstraint(const Vector3& center, float radius) : Constraint(ConstraintType::Attach, center, radius) {}
      };
   }

   class ClothSimulation;

   class ClothMesh;
   using ClothMeshRef = sptr<ClothMesh>;

   class ClothMesh : public BaseMesh {
      friend ClothSimulation;
   public:
      ClothMesh(uint m, uint n, const Matrix4& meshTransform, const std::string& name = "");

      static ClothMeshRef Create(uint m, uint n, const Matrix4& transform = Matrix4::Identity);

      void RebuildMesh(uint m, uint n, bool force = false);

      StructuredBuffer& GetPositionBuffer();
      StructuredBuffer& GetVelocityBuffer();
      StructuredBuffer& GetNormalBuffer();

      StructuredBuffer& GetPositionTmpBuffer(uint ind);

      StructuredBuffer& GetConstraintsBuffer();

      void PrepareDrawBuffers(CommandContext& context) override;
      void SetDrawBuffers(GraphicsContext& context) override;

      const uint GetDrawIndexCount() const override;

      uint GetM() const { return m_m; }
      uint GetN() const { return m_n; }

      float GetWidth() const { return m_width; }
      float GetHeight() const { return m_height; }

      void InitConstrainsBuffer(uint maxConstrains);
      void SetConstrains(const std::vector<ClothConstraint::Constraint>& constraints);
      const std::vector<ClothConstraint::Constraint>& GetConstrains() const;

   private:
      MeshData m_meshData;

      StructuredBuffer m_posBuffer;
      StructuredBuffer m_normalsBuffer;
      StructuredBuffer m_tangentBuffer;
      StructuredBuffer m_velocityBuffer;

      StructuredBuffer m_posTmpBuffer[2];

      StructuredBuffer m_constraintsBuffer;
      bool m_isConstrainsDirty;

      StructuredBuffer m_texBuffer;

      ByteAddressBuffer m_indexBuffer;
      uint m_m, m_n;
      uint m_width, m_height;

      Matrix4 m_meshTransform;

      std::vector<ClothConstraint::Constraint> m_constraints;

      void CreateGPUBuffers();
   };
   

   class ClothSimulation {
   public:
      ClothSimulation();
      bool Init();
      bool RebuildShaderAndPSO();

      void AddSimCloth(ClothMeshRef& cloth);
      void Update(ComputeContext& context, Timestep ts);

      int m_iter = 16;
      bool m_solvePass;
      float m_deltaRimeMultiplier;

      float gKVelocityDump = 0.97f;
      float gKs = 0.071f;
      float gKs_diagonal = 0.071f;
      float gKs_bend = 0.022f;
      bool gUseDiagonal = true;
      bool gUseBend = true;

   private:
      bool m_inited;
      RootSignature m_rootSignature;
      ComputePSO m_psoPrepare;
      ComputePSO m_psoSolve;
      ComputePSO m_psoRecord;
      ComputePSO m_psoComputeNormal;

      StructuredBuffer m_cbComputePassBuffer;

      std::vector<ClothMeshRef> m_simClothes;

      ShaderRef m_cShader;

      bool CreateShaders();
      bool CreatePSO();
   };
}
