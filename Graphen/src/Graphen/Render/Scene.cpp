#include "gnpch.h"
#include "Scene.h"
#include "Graphen/Core/Camera.h"

namespace gn {
   void Scene::Draw(GraphicsContext& context, const Camera& camera) const {
      for (auto&& pModel : m_models) {
         if (!pModel || !pModel->m_effect) {
            GN_CORE_WARN("[SCENE] Model or model's effect is nullptr");
            continue;
         }
         Model& model = *pModel;
         Effect& effect = *model.m_effect;

         effect.Apply(context, PASS_NAME_OPAQUE);

         // todo: crate cb for light
         context.SetDynamicConstantBufferView(1, sizeof(Light), &m_lights[0]);

         CB_ALIGN sb::CBPass cbFrame;
         cbFrame.g_time = 0.f; // todo:
         cbFrame.eye = camera.GetPosition();
         cbFrame.viewProj = camera.GetViewProjMatrix();

         context.SetDynamicConstantBufferView(0, sizeof(cbFrame), &cbFrame);

         model.Mesh->SetGeometry(context);
         context.SetBufferSRV(2, model.InstanceData);
         context.DrawIndexedInstanced(model.Mesh->IndexesCount(), model.Transforms.size(), 0, 0, 0);
      }
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
