#pragma once
#include "CommandContext.h"
#include "Shader.h"


namespace gn {
   enum class PassID : uint {
      ZPrePass,
      ZPass,
      Opaque,
   };

   const char PASS_NAME_OPAQUE[] = "opaque";
   const char PASS_NAME_Z_PASS[] = "zPass";
   const char PASS_NAME_PRERECORD[] = "prerecord";

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
}
