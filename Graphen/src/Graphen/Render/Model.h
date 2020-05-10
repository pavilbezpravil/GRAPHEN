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
      Model(const sptr<BaseMesh>& mesh, const EffectRef& effect, const Matrix4& transform, const Matrix4& worldTransform = Matrix4::Identity)
         : m_mesh(mesh), m_effect(effect), m_transforms(), m_worldTransform(worldTransform) {
         m_transforms.push_back(transform);
         CreateBuffers();
      }

      Model(const sptr<BaseMesh>& mesh, const EffectRef& effect, const std::vector<Matrix4>& transforms, const Matrix4& worldTransform = Matrix4::Identity)
         : m_mesh(mesh), m_effect(effect), m_transforms(transforms), m_worldTransform(worldTransform) {
         CreateBuffers();
      }

      static ModelRef Create(const sptr<BaseMesh>& mesh, const EffectRef& effect, const std::vector<Matrix4>& transforms, const Matrix4& worldTransform = Matrix4::Identity) {
         return CreateRef<Model>(mesh, effect, transforms, worldTransform);
      }

      static ModelRef Create(const sptr<BaseMesh>& mesh, const EffectRef& effect, const Matrix4& transform, const Matrix4& worldTransform = Matrix4::Identity) {
         return Create(mesh, effect, std::vector<Matrix4>{transform}, worldTransform);
      }

      const uint GetInstanceCount() const { return m_transforms.size(); }

      const bool IsValidMesh() const { return !!m_mesh; }
      const bool IsValidEffect() const { return !!m_effect; }

      BaseMesh& GetMesh() { return *m_mesh; }
      Effect& GetEffect() { return *m_effect; }

      StructuredBuffer& GetInstanceData() { return m_instanceData; }

      const Matrix4 GetTransform() const { return m_transforms[0] * m_worldTransform; }

   private:
      void CreateBuffers() {
         if (m_transforms.empty()) {
            return;
         }
         std::vector<sb::InstanceData> instanceDatas(m_transforms.size());
         for (int i = 0; i < m_transforms.size(); ++i) {
            instanceDatas[i].model = m_transforms[i] * m_worldTransform;
            instanceDatas[i].modelNormal = Transpose(Invert(instanceDatas[i].model));
         }

         m_instanceData.Create(L"", (uint)m_transforms.size(), sizeof(sb::InstanceData), instanceDatas.data());
      }

      BaseMeshRef m_mesh;
      std::vector<Matrix4> m_transforms;
      Matrix4 m_worldTransform;
      StructuredBuffer m_instanceData;

      EffectRef m_effect;
   };
}
