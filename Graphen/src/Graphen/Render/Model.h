#pragma once

#include "BaseMesh.h"
#include "Effect.h"


namespace gn {

   // shader buffers
   namespace sb {
      struct InstanceData {
         Matrix4 model;
         Matrix4 modelNormal;
      };
   }

   class Model;
   using ModelRef = Ref<Model>;

   class Model {
   public:

      Model(const sptr<BaseMesh>& mesh, const EffectRef& effect, const std::vector<Matrix4>& transforms, const Matrix4& worldTransform = Matrix4::Identity)
         : m_mesh(mesh), m_effect(effect), m_transforms(transforms), m_worldTransform(worldTransform), m_instanceDataDirty(true), m_enable(true) {
         CreateBuffers();
      }

      Model(const sptr<BaseMesh>& mesh, const EffectRef& effect, const Matrix4& transform, const Matrix4& worldTransform = Matrix4::Identity)
         : Model(mesh, effect, std::vector<Matrix4>{transform}, worldTransform) {}

      static ModelRef Create(const sptr<BaseMesh>& mesh, const EffectRef& effect, const std::vector<Matrix4>& transforms, const Matrix4& worldTransform = Matrix4::Identity) {
         return CreateRef<Model>(mesh, effect, transforms, worldTransform);
      }

      static ModelRef Create(const sptr<BaseMesh>& mesh, const EffectRef& effect, const Matrix4& transform, const Matrix4& worldTransform = Matrix4::Identity) {
         return Create(mesh, effect, std::vector<Matrix4>{transform}, worldTransform);
      }

      void PrepareDraw(CommandContext& context);
      void Draw(GraphicsContext& context) const;

      const uint GetInstanceCount() const { return m_transforms.size(); }

      const bool IsValidMesh() const { return !!m_mesh; }
      const bool IsValidEffect() const { return !!m_effect; }

      BaseMesh& GetMesh() { return *m_mesh; }
      Effect& GetEffect() { return *m_effect; }

      const StructuredBuffer& GetInstanceData() const { return m_instanceData; }
      StructuredBuffer& GetInstanceData() { return m_instanceData; }

      void UpdateInstanceData(CommandContext& context);

      const Matrix4 GetTransform() const { return m_transforms[0] * m_worldTransform; }
      void SetWorldMatrix(const Matrix4& world);
      const Matrix4& GetWorldMatrix() { return m_worldTransform; }

      void SetEnable(bool enable);
      bool GetEnable() const { return m_enable; }

   private:
      void CreateBuffers() {
         if (m_transforms.empty()) {
            return;
         }
         m_instanceData.Create(L"", (uint)m_transforms.size(), sizeof(sb::InstanceData), nullptr);
      }

      BaseMeshRef m_mesh;
      std::vector<Matrix4> m_transforms;
      Matrix4 m_worldTransform;
      StructuredBuffer m_instanceData;
      bool m_instanceDataDirty;

      bool m_enable;

      EffectRef m_effect;
   };
}
