#pragma once

#include "Graphen.h"
#include "Graphen/Core/Camera.h"
#include "Graphen/Core/CameraController.h"
#include "Graphen/Render/RootSignature.h"
#include "Graphen/Render/GraphicsCore.h"
#include "Graphen/Render/Shader.h"
#include "Graphen/Render/GpuBuffer.h"
#include "Graphen/Render/DepthBuffer.h"

class ExampleLayer : public gn::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(gn::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(gn::Event& e) override;

   void Render();
private:
   RootSignature m_RootSignature;

   GraphicsPSO m_TrianglePSO;

   DepthBuffer m_depth;
   ColorBuffer m_color;

   sptr<Shader> vertexShader;
   sptr<Shader> pixelShader;

   ByteAddressBuffer m_VertexBuffer;
   ByteAddressBuffer m_IndexBuffer;

   Camera m_Camera;
   uptr<GameCore::CameraController> m_CameraController;
};
