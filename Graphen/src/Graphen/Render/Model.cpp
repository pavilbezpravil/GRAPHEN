#include "gnpch.h"
#include "Model.h"

namespace gn {
   void Model::SetWorldMatrix(const Matrix4& world) {
      m_worldTransform = world;
      m_instanceDataDirty = true;
   }

   void Model::SetEnable(bool enable) {
      m_enable = enable;
   }

   void Model::PrepareDraw(CommandContext& context) {
      context.TransitionResource(GetInstanceData(), D3D12_RESOURCE_STATE_GENERIC_READ);
      m_mesh->PrepareDrawBuffers(context);
   }

   void Model::Draw(GraphicsContext& context) const {
      m_mesh->SetDrawBuffers(context);
      context.SetBufferSRV(2, GetInstanceData());
      context.DrawIndexedInstanced(m_mesh->GetDrawIndexCount(), GetInstanceCount(), 0, 0, 0);
   }

   void Model::UpdateInstanceData(CommandContext& context) {
      if (m_instanceDataDirty) {
         std::vector<sb::InstanceData> instanceDatas(m_transforms.size());
         for (int i = 0; i < m_transforms.size(); ++i) {
            instanceDatas[i].model = m_transforms[i] * m_worldTransform;
            instanceDatas[i].modelNormal = Transpose(Invert(instanceDatas[i].model));
         }
         context.WriteBuffer(m_instanceData, 0, instanceDatas.data(), instanceDatas.size() * sizeof(sb::InstanceData));
         context.BeginResourceTransition(m_instanceData, D3D12_RESOURCE_STATE_COMMON);
         m_instanceDataDirty = false;
      }
   }
}
