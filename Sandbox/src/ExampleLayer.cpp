#include "ExampleLayer.h"

#include "imgui/imgui.h"
#include "Graphen/Render/CommandContext.h"
#include "Graphen/Render/GeometryGenerator.h"
using namespace GameCore;
using namespace Graphics;
using namespace gn;

namespace {
   class Model
   {
   public:
      Model(const sptr<Mesh>& mesh, const Matrix4& transform)
            : Mesh(mesh), Transform(transform) { }

      sptr<Mesh> Mesh;
      Matrix4 Transform;
   };

   std::vector<Model> s_models;
}

ExampleLayer::ExampleLayer() 
	: Layer("ExampleLayer")
{
	
}

void ExampleLayer::OnAttach()
{
   BuildShadersAndPSO();

   s_models.push_back({ Mesh::CreateFromMeshData(GeometryGenerator::CreateGrid(50, 50, 2, 2)), Matrix4::CreateTranslation(0, -1, 0) });
   s_models.push_back({ Mesh::CreateFromMeshData(GeometryGenerator::CreateBox(1, 1, 1, 1)), Matrix4::CreateTranslation(-2, 0, 0) });
   s_models.push_back({ Mesh::CreateFromMeshData(GeometryGenerator::CreateGeosphere(0.5f, 3)), Matrix4::CreateTranslation(0, 0, 0) });
   s_models.push_back({ Mesh::CreateFromMeshData(GeometryGenerator::CreateCylinder(0.5f, 0.1f, 1, 32, 2)), Matrix4::CreateTranslation(2, 0, 0) });
   
   const Vector3 eye = Vector3(0, 0, 2.f);
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
   if (!m_vertexShader || !m_pixelShader)
   {
      return;
   }

   renderer.GetLDRTarget().SetClearColor(m_clearColor);

   ColorBuffer& colorBuffer = renderer.GetLDRTarget();
   DepthBuffer& depthBuffer = renderer.GetDepth();

   GraphicsContext& context = GraphicsContext::Begin(L"Scene Render");

   context.TransitionResource(colorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
   context.TransitionResource(depthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
   context.ClearColor(colorBuffer);
   context.ClearDepth(depthBuffer);

   context.SetPipelineState(m_modelPSO);
   context.SetRootSignature(m_rootSignature);
   context.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
   context.SetRenderTarget(colorBuffer.GetRTV(), depthBuffer.GetDSV());
   context.SetViewportAndScissor(0, 0, colorBuffer.GetWidth(), colorBuffer.GetHeight());

   struct CBFrame
   {
      float g_time;
      Vector3 eye;
      Matrix4 viewProj;
   };

   struct CBPerInstance
   {
      Matrix4 model;
      Matrix4 modelNormal;
   };

   struct Light
   {
      Vector3 position;
      float pad1;
      Vector3 color;
      float pad2;

      Light() = default;

      Light(const Vector3& position, const Vector3& color)
         : position(position),
         color(color)
      {
      }
   };

   CB_ALIGN CBFrame cbFrame;
   cbFrame.g_time = 0.f; // todo:
   cbFrame.eye = m_camera.GetPosition();
   cbFrame.viewProj = m_camera.GetViewProjMatrix();

   CB_ALIGN struct {
      Light light;
   } cbLight;
   cbLight.light = { Vector3(0, 2, 2), Vector3(1, 1, 1) };

   context.SetDynamicConstantBufferView(0, sizeof(cbFrame), &cbFrame);
   context.SetDynamicConstantBufferView(1, sizeof(cbLight), &cbLight);

   for (auto && model : s_models)
   {
      model.Mesh->SetGeometry(context);

      CB_ALIGN CBPerInstance cbPerInstance;
      cbPerInstance.model = model.Transform;
      cbPerInstance.modelNormal = Transpose(Invert(cbPerInstance.model));

      context.SetDynamicConstantBufferView(2, sizeof(cbPerInstance), &cbPerInstance);

      context.DrawIndexed(model.Mesh->IndexesCount());
   }

   context.Finish();
}

void ExampleLayer::OnImGuiRender() 
{
	ImGui::Begin("Test");

   static bool showDemo = true;
   ImGui::Checkbox("Show Demo Window", &showDemo);

   ImGui::ColorEdit4("Clear Color", m_clearColor.GetPtr());

   ImGui::End();

   if (showDemo) {
      ImGui::ShowDemoWindow(&showDemo);
   }
}

void ExampleLayer::OnEvent(gn::Event& e) 
{
   gn::EventDispatcher dispatcher(e);
   dispatcher.Dispatch<gn::KeyPressedEvent>([&] (gn::KeyPressedEvent& e)
   {
      if (e.GetKeyCode() == HZ_KEY_T)
      {
         if (gn::Input::IsKeyPressed(HZ_KEY_T)) {
            m_cameraController->Enable(!m_cameraController->Enable());
            gn::Application::Get().GetWindow().ShowCursor(!m_cameraController->Enable());
         }
      }
      if (e.GetKeyCode() == HZ_KEY_R)
      {
         BuildShadersAndPSO();
      }
      return false;
   });
}

void ExampleLayer::BuildShadersAndPSO()
{
   {
      auto vShader = Shader::Create("../Shaders/base.hlsl", "baseVS", ShaderType::Vertex);      
      auto pShader = Shader::Create("../Shaders/base.hlsl", "basePS", ShaderType::Pixel);
      if (vShader && pShader)
      {
         m_vertexShader = vShader;
         m_pixelShader = pShader;
      } else
      {
         return;
      }
   }

   m_rootSignature.Reset(3, 0);
   m_rootSignature[0].InitAsConstantBuffer(0);
   m_rootSignature[1].InitAsConstantBuffer(1);
   m_rootSignature[2].InitAsConstantBuffer(4);
   m_rootSignature.Finalize(L"Triangle", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

   D3D12_INPUT_ELEMENT_DESC vertElem[] =
   {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
   };

   m_modelPSO.SetRootSignature(m_rootSignature);
   m_modelPSO.SetRasterizerState(RasterizerDefault);
   m_modelPSO.SetBlendState(BlendDisable);
   m_modelPSO.SetDepthStencilState(DepthStateReadWrite);
   m_modelPSO.SetInputLayout(_countof(vertElem), vertElem);
   m_modelPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
   m_modelPSO.SetRenderTargetFormat(gn::Application::Get().GetRenderer().GetLDRFormat(), gn::Application::Get().GetRenderer().GetDepthFormat());
   m_modelPSO.SetVertexShader(m_vertexShader->GetBytecode());
   m_modelPSO.SetPixelShader(m_pixelShader->GetBytecode());
   m_modelPSO.Finalize();
}
