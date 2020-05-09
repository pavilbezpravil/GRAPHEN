#include "gnpch.h"
#include "Scene.h"
#include "Graphen/Core/Camera.h"
#include "Graphen/Render/Renderer.h"

namespace gn {

   namespace {
      const uint SCENE_MAX_LIGHT = 16;

      CB_ALIGN struct CBLight {
         DirectionalLight directionalLight;
         Light light[SCENE_MAX_LIGHT];
      };
   }

   void Scene::Draw(Renderer& renderer, GraphicsContext& context, const BaseCamera& camera, const ShadowCamera& shadowCamera, const char* tech) const {
      for (auto&& pModel : m_models) {
         if (!pModel || !pModel->IsValidMesh() || !pModel->IsValidEffect()) {
            GN_CORE_WARN("[SCENE] Model or model's mesh or model's effect is invalid");
            continue;
         }
         Model& model = *pModel;
         Effect& effect = model.GetEffect();
         BaseMesh& mesh = model.GetMesh();

         effect.Apply(context, tech);

         CBLight cbLights;
         cbLights.directionalLight = m_directionalLight;
         memcpy(cbLights.light, m_lights.data(), sizeof(Light) * m_lights.size());

         // todo: create cb for light
         context.SetDynamicConstantBufferView(1, sizeof(CBLight), &cbLights);

         CB_ALIGN sb::CBPass cbFrame;
         cbFrame.g_time = 0.f; // todo:
         cbFrame.eye = camera.GetPosition();
         cbFrame.viewProj = camera.GetViewProjMatrix();
         cbFrame.modelToShadow = shadowCamera.GetShadowMatrix();

         context.SetDynamicConstantBufferView(0, sizeof(cbFrame), &cbFrame);
         context.SetDynamicDescriptor(3, 0, renderer.GetShadow().GetSRV());

         context.SetVertexBuffer(0, mesh.GetVertexBufferForDraw().VertexBufferView());
         context.SetIndexBuffer(mesh.GetIndexBufferForDraw().IndexBufferView());
         context.SetBufferSRV(2, model.GetInstanceData());
         context.DrawIndexedInstanced(mesh.GetIndexBufferForDraw().GetElementCount(), model.GetInstanceCount(), 0, 0, 0);
      }
   }

   void Scene::AddDirectionalLight(const DirectionalLight& light) {
      m_directionalLight = light;
   }

   void Scene::AddLight(const Light& light) {
      m_lights.push_back(light);
   }

   void Scene::AddModel(const sptr<Model>& model) {
      if (model) {
         m_models.push_back(model);
      }
   }
}
