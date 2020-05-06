#pragma once

#include "Graphen.h"
#include "Graphen/Core/Camera.h"
#include "Graphen/Core/CameraController.h"
#include "Graphen/Render/Scene.h"

class ExampleLayer : public gn::Layer
{
public:
	ExampleLayer();
   virtual ~ExampleLayer();

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(gn::Timestep ts) override;
	void OnRender(gn::Renderer& renderer) override;
	void OnImGuiRender() override;
	void OnEvent(gn::Event& e) override;

   void ToggleCameraControl();

private:
   gn::EffectRef m_effect;

   void BuildShadersAndPSOForType(const std::string& type);
   void BuildShadersAndPSO();

   gn::Scene m_scene;

   Vector3 m_sunDirection;

   Camera m_camera;
   uptr<GameCore::CameraController> m_cameraController;
};
