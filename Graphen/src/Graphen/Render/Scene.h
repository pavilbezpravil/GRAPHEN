#pragma once

#include "Model.h"


namespace Math {
   class ShadowCamera;
   class BaseCamera;
}


namespace gn {
   class Renderer;
   // shader buffers
   namespace sb {
      struct CBPass {
         float g_time;
         Vector3 eye;
         Matrix4 viewProj;
         Matrix4 modelToShadow;
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

   class Scene {
   public:
      using ModelVec = std::vector<sptr<Model>>;
      using LightVec = std::vector<Light>;

      void Prepare(GraphicsContext& context) const;
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
