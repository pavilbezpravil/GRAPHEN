#pragma once

#include "Graphen.h"
#include "Graphen/Core/Camera.h"
#include "Graphen/Core/CameraController.h"
#include "Graphen/Render/Scene.h"
#include "Graphen/Render/Cloth.h"

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
   gn::EffectRef m_effectCloth;

   void BuildShadersAndPSO();
   void BuildShadersAndPSOForPass(gn::EffectRef& effect, const std::string& type, const std::vector<D3D_SHADER_MACRO>& macros = {});

   gn::Scene m_scene;

   gn::ClothSimulation m_clothSimulation;
   gn::ClothMeshRef m_clothMesh;
   gn::ModelRef m_clothModel;
   std::vector<gn::ModelRef> m_clothSpheres;

   Vector3 m_sunDirection;

   bool m_wireframe = false;

   bool m_enableExternalObjects = true;

   Camera m_camera;
   uptr<GameCore::CameraController> m_cameraController;
};
