#pragma once

#include "Graphen.h"
#include "Graphen/Core/Camera.h"
#include "Graphen/Core/CameraController.h"
#include "Graphen/Render/RootSignature.h"
#include "Graphen/Render/Shader.h"
#include "Graphen/Render/DepthBuffer.h"
#include "Graphen/Render/PipelineState.h"

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
   RootSignature m_rootSignature;
   GraphicsPSO m_modelPSO;

   sptr<Shader> m_vertexShader;
   sptr<Shader> m_pixelShader;

   void BuildShadersAndPSO();

   Color m_clearColor;

   Camera m_camera;
   uptr<GameCore::CameraController> m_cameraController;
};
