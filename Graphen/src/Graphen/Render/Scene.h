#pragma once

#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "GpuBuffer.h"
#include "GeometryGenerator.h"
#include "Shader.h"


namespace Math {
   class Camera;
}


namespace gn {
   // shader buffers
   namespace sb {
      struct CBPass {
         float g_time;
         Vector3 eye;
         Matrix4 viewProj;
      };

      struct InstanceData {
         Matrix4 model;
         Matrix4 modelNormal;
      };
   }

   struct Light {
      Vector3 position;
      float pad1;
      Color color;

      Light() = default;

      Light(const Vector3& position, const Vector3& color)
         : position(position),
         color(color) {
      }
   };

   const char PASS_NAME_OPAQUE[] = "opaque";
   const char PASS_NAME_Z_PASS[] = "zPass";

   class Effect {
   public:
      virtual ~Effect() = default;

      operator bool() const { return true; } // todo: tmp

      void Apply(GraphicsContext& context, const char* tech) {
         context.SetPipelineState(*m_modelPSO[tech]);
         context.SetRootSignature(*m_rootSignature[tech]);
         context.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      }

      std::unordered_map<std::string, sptr<RootSignature>> m_rootSignature;
      std::unordered_map<std::string, sptr<GraphicsPSO>> m_modelPSO;

      std::unordered_map<std::string, sptr<Shader>> m_vertexShader;
      std::unordered_map<std::string, sptr<Shader>> m_pixelShader;
   };
   using EffectRef = Ref<Effect>;

   class Model {
   public:
      Model(const sptr<Mesh>& mesh, const EffectRef& effect, const Matrix4& transform, const Matrix4& worldTransform = Matrix4::Identity)
         : Mesh(mesh), m_effect(effect), Transforms(), WorldTransform(worldTransform) {
         Transforms.push_back(transform);
         CreateBuffers();
      }

      Model(const sptr<Mesh>& mesh, const EffectRef& effect, const std::vector<Matrix4>& transforms, const Matrix4& worldTransform = Matrix4::Identity)
         : Mesh(mesh), m_effect(effect), Transforms(transforms), WorldTransform(worldTransform) {
         CreateBuffers();
      }

      void CreateBuffers() {
         if (Transforms.empty()) {
            return;
         }
         std::vector<sb::InstanceData> instanceDatas(Transforms.size());
         for (int i = 0; i < Transforms.size(); ++i) {
            instanceDatas[i].model = Transforms[i] * WorldTransform;
            instanceDatas[i].modelNormal = Transpose(Invert(instanceDatas[i].model));
         }

         InstanceData.Create(L"", (uint)Transforms.size(), sizeof(sb::InstanceData), instanceDatas.data());
      }

      MeshRef Mesh;
      std::vector<Matrix4> Transforms;
      Matrix4 WorldTransform;
      StructuredBuffer InstanceData;

      EffectRef m_effect;
   };

   class Scene {
   public:
      using ModelVec = std::vector<sptr<Model>>;
      using LightVec = std::vector<Light>;

      void Draw(GraphicsContext& context, const Camera& camera, const char* tech) const;

      void AddLight(const Light& light);
      void AddModel(const sptr<Model>& model);

      const ModelVec& GetModels() const { return m_models; }
      const LightVec& GetLights() const { return m_lights; }

   private:
      ModelVec m_models;
      LightVec m_lights;
   };
}
