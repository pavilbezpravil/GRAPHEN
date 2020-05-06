#pragma once

#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "GpuBuffer.h"
#include "GeometryGenerator.h"
#include "Shader.h"


namespace Math {
   class ShadowCamera;
   class BaseCamera;
}


namespace gn {
   class Renderer; // shader buffers
   namespace sb {
      struct CBPass {
         float g_time;
         Vector3 eye;
         Matrix4 viewProj;
         Matrix4 modelToShadow;
      };

      struct InstanceData {
         Matrix4 model;
         Matrix4 modelNormal;
      };
   }

   // it should be consist with shaders definition
   enum class LightType : int {
      Invalid = -1,
      Directional = 0,
      Point,
      Spot,
   };

   struct Light {
      Vector3 positionOrDir;
      LightType type;
      Color color;
      float strength;

      Light() : type(LightType::Invalid) {}

      Light(LightType type, const Vector3& position, const Vector3& color, float strength)
            : type(type), positionOrDir(position), color(color), strength(strength) {}
   };

   struct DirectionalLight : Light {
      DirectionalLight() : Light() {}
      DirectionalLight(const Vector3& dir, const Vector3& color, float strength)
      : Light(LightType::Directional, dir, color, strength) {}

      const Vector3& GetDirection() const { return positionOrDir; }
   };

   const char PASS_NAME_OPAQUE[] = "opaque";
   const char PASS_NAME_Z_PASS[] = "zPass";

   class Effect {
   public:
      virtual ~Effect() = default;

      operator bool() const { return CheckTech(PASS_NAME_OPAQUE) && CheckTech(PASS_NAME_Z_PASS); }

      void Apply(GraphicsContext& context, const char* tech) {
         context.SetPipelineState(*m_modelPSO[tech]);
         context.SetRootSignature(*m_rootSignature[tech]);
         context.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      }

      std::unordered_map<std::string, sptr<RootSignature>> m_rootSignature;
      std::unordered_map<std::string, sptr<GraphicsPSO>> m_modelPSO;

      std::unordered_map<std::string, sptr<Shader>> m_vertexShader;
      std::unordered_map<std::string, sptr<Shader>> m_pixelShader;

   private:
      bool CheckTech(const char* tech) const {
         return
            m_rootSignature.count(tech) && m_rootSignature.at(tech) &&
            m_modelPSO.count(tech) && m_modelPSO.at(tech) &&
            m_vertexShader.count(tech) && m_vertexShader.at(tech) &&
            m_pixelShader.count(tech) && m_pixelShader.at(tech);
      }
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

      // todo: shadow camera
      void Draw(Renderer& renderer, GraphicsContext& context, const BaseCamera& camera, const ShadowCamera& shadowCamera, const char* tech) const;

      void AddDirectionalLight(const DirectionalLight& light);
      void AddLight(const Light& light);
      void AddModel(const sptr<Model>& model);

      const ModelVec& GetModels() const { return m_models; }
      const LightVec& GetLights() const { return m_lights; }
      const DirectionalLight& GetDirectionalLight() const { return m_directionalLight; }

   private:
      ModelVec m_models;
      LightVec m_lights;
      DirectionalLight m_directionalLight;
   };
}
