#pragma once

#include "Graphen.h"
#include "Graphen/Core/Camera.h"
#include "Graphen/Core/CameraController.h"
#include "Graphen/Render/RootSignature.h"
#include "Graphen/Render/PipelineState.h"
#include "Graphen/Render/Shader.h"

class ExampleLayer : public gn::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(gn::Timestep ts) override;
	void OnRender(gn::Renderer& renderer) override;
	void OnImGuiRender() override;
	void OnEvent(gn::Event& e) override;

private:
   std::unordered_map<std::string, RootSignature> m_rootSignature;
   std::unordered_map<std::string, GraphicsPSO> m_modelPSO;

   std::unordered_map<std::string, sptr<Shader>> m_vertexShader;
   std::unordered_map<std::string, sptr<Shader>> m_pixelShader;

   void BuildShadersAndPSOForType(const std::string& type);
   void BuildShadersAndPSO();

   Camera m_camera;
   uptr<GameCore::CameraController> m_cameraController;
};
