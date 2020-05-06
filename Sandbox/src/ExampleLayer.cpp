#include "ExampleLayer.h"

#include "imgui/imgui.h"
#include "Graphen/Render/CommandContext.h"
#include "Graphen/Render/GeometryGenerator.h"
#include "Graphen/Render/SamplerManager.h"

using namespace gn;
using namespace GameCore;
using namespace Graphics;


ExampleLayer::ExampleLayer() 
	: Layer("ExampleLayer")
{
	
}

ExampleLayer::~ExampleLayer() {
}

void ExampleLayer::OnAttach()
{
   m_effect = std::make_shared<Effect>();
   BuildShadersAndPSO();

   float dist = 2.f;
   uint n = 10;
   std::vector<Matrix4> pos(n * n);
   for (uint z = 0; z < n; ++z) {
      for (uint x = 0; x < n; ++x) {
         pos[n * z + x] = Matrix4::CreateTranslation(x * dist, 0, z * dist);
      }
   }

   m_sunDirection = Vector3(0.12, -1, 0.53f);
   // m_sunDirection = Vector3(0, -1, 0);
   DirectionalLight dirLight = { m_sunDirection, Vector3(1, 1, 1), 1 };
   m_scene.AddDirectionalLight(dirLight);
   // m_scene.AddLight({ Vector3(0, 3, 0), Vector3(1, 1, 1) });

   m_scene.AddModel(std::make_shared<Model>( Mesh::CreateFromMeshData(GeometryGenerator::CreateGrid(50, 50, 2, 2)), m_effect,
      Matrix4::CreateTranslation(0, 0, 0) ));

   m_scene.AddModel(std::make_shared<Model>( Mesh::CreateFromMeshData(GeometryGenerator::CreateBox(1, 1, 1, 1)), m_effect,
      pos, Matrix4::CreateTranslation(0, 1, 0) ));
   m_scene.AddModel(std::make_shared<Model>( Mesh::CreateFromMeshData(GeometryGenerator::CreateCylinder(0.5f, 0.1f, 1, 32, 2)), m_effect,
      pos, Matrix4::CreateTranslation(0, 2, 0) ));
   m_scene.AddModel(std::make_shared<Model>( Mesh::CreateFromMeshData(GeometryGenerator::CreateGeosphere(0.5f, 3)), m_effect,
      pos, Matrix4::CreateTranslation(0, 3, 0) ));

   const Vector3 eye = Vector3(0, 1, -2.f);
   m_camera.SetEyeAtUp(eye, Vector3::Zero, Vector3::UnitY);
   m_camera.SetZRange(0.1f, 100.0f);
   m_camera.SetFOV(0.25f * XM_PI);
   m_camera.SetAspectRatio(1080.f / 1920.f);
   m_cameraController.reset(new CameraController(m_camera, Vector3::UnitY));
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(gn::Timestep ts)
{
   m_cameraController->Update(ts.GetSeconds());
}

void ExampleLayer::OnRender(gn::Renderer& renderer)
{
   if (!m_effect || !*m_effect)
   {
      return;
   }

   renderer.DrawScene(m_scene, m_camera);
}

void ExampleLayer::OnImGuiRender() 
{
   static float nearZ;
   static float farZ;

   ImGui::Begin("Camera");
   ImGui::SliderFloat("near z", &nearZ, 0, 10, "%.3f", 2);
   ImGui::SliderFloat("far z", &farZ, 11, 100, "%.3f", 2);
   ImGui::End();

   m_camera.SetZRange(nearZ, farZ);
}

void ExampleLayer::OnEvent(gn::Event& e) 
{
   gn::EventDispatcher dispatcher(e);
   dispatcher.Dispatch<gn::KeyPressedEvent>([&] (gn::KeyPressedEvent& e) {
      if (e.GetKeyCode() == HZ_KEY_T) {
         ToggleCameraControl();
      } else if (e.GetKeyCode() == HZ_KEY_R) {
         BuildShadersAndPSO();
      }
      return false;
   });
   dispatcher.Dispatch<gn::AppKillFocusEvent>([&](AppKillFocusEvent& _e) {
      if (m_cameraController->Enable()) {
         ToggleCameraControl();
      }
      return false;
   });
   dispatcher.Dispatch<gn::WindowResizeEvent>([&](WindowResizeEvent& e) {
      m_camera.SetAspectRatio(e.GetHeight() / e.GetWidth());
      return false;
   });
}

void ExampleLayer::ToggleCameraControl() {
   m_cameraController->Enable(!m_cameraController->Enable());
   gn::Application::Get().GetWindow().ShowCursor(!m_cameraController->Enable());
}

void ExampleLayer::BuildShadersAndPSOForType(const std::string& type) {
   if (!m_effect->m_vertexShader.count(type)) {
      m_effect->m_vertexShader[type] = {};
   }
   if (!m_effect->m_pixelShader.count(type)) {
      m_effect->m_pixelShader[type] = {};
   }

   auto& vertexShader = m_effect->m_vertexShader[type];
   auto& pixelShader = m_effect->m_pixelShader[type];

   {
      std::vector<D3D_SHADER_MACRO> defines;
      if (type == PASS_NAME_Z_PASS) {
         defines.push_back({"Z_PASS", nullptr});
      }
      defines.push_back({ nullptr, nullptr });

      auto vShader = Shader::Create("../Shaders/base.hlsl", "baseVS", ShaderType::Vertex, defines.data());
      auto pShader = Shader::Create("../Shaders/base.hlsl", "basePS", ShaderType::Pixel, defines.data());
      if (vShader && pShader) {
         vertexShader = vShader;
         pixelShader = pShader;
      } else {
         return;
      }
   }

   if (!m_effect->m_rootSignature.count(type)) {
      m_effect->m_rootSignature[type] = CreateRef<RootSignature>();
   }
   if (!m_effect->m_modelPSO.count(type)) {
      m_effect->m_modelPSO[type] = CreateRef<GraphicsPSO>();
   }

   auto& rootSignature = *m_effect->m_rootSignature[type];
   auto& pso = *m_effect->m_modelPSO[type];

   rootSignature.Reset(4, 1);
   rootSignature.InitStaticSampler(0, SamplerPointBorderDesc);
   rootSignature[0].InitAsConstantBuffer(0);
   rootSignature[1].InitAsConstantBuffer(1);
   rootSignature[2].InitAsBufferSRV(0, 1);
   rootSignature[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1); // shadow map
   rootSignature.Finalize(L"Model", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

   D3D12_INPUT_ELEMENT_DESC vertElem[] =
   {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
   };

   D3D12_DEPTH_STENCIL_DESC depthState = DepthStateReadWrite;
   if (type == PASS_NAME_OPAQUE) {
      depthState = DepthStateTestEqual;
   }

   pso.SetRootSignature(rootSignature);
   pso.SetRasterizerState(RasterizerDefault);
   pso.SetBlendState(BlendDisable);
   pso.SetDepthStencilState(depthState);
   pso.SetInputLayout(_countof(vertElem), vertElem);
   pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
   if (type == PASS_NAME_OPAQUE) {
      pso.SetRenderTargetFormat(gn::Application::Get().GetRenderer().GetLDRFormat(), gn::Application::Get().GetRenderer().GetDepthFormat());
   }
   if (type == PASS_NAME_Z_PASS) {
      pso.SetRenderTargetFormats(0, nullptr, gn::Application::Get().GetRenderer().GetDepthFormat());
   }
   pso.SetVertexShader(vertexShader->GetBytecode());
   pso.SetPixelShader(pixelShader->GetBytecode());
   pso.Finalize();
}

void ExampleLayer::BuildShadersAndPSO() {
   BuildShadersAndPSOForType(PASS_NAME_OPAQUE);
   BuildShadersAndPSOForType(PASS_NAME_Z_PASS);
}
