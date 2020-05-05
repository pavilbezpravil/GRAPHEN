#include "ExampleLayer.h"

#include "imgui/imgui.h"
#include "Graphen/Render/CommandContext.h"
#include "Graphen/Render/GeometryGenerator.h"
using namespace gn;
using namespace GameCore;
using namespace Graphics;

namespace {
   // shader buffers
   namespace sb {
      struct CBFrame {
         float g_time;
         Vector3 eye;
         Matrix4 viewProj;
      };

      struct Light {
         Vector3 position;
         float pad1;
         Vector3 color;
         float pad2;

         Light() = default;

         Light(const Vector3& position, const Vector3& color)
            : position(position),
            color(color) {
         }
      };

      struct InstanceData {
         Matrix4 model;
         Matrix4 modelNormal;
      };
   }

   class Model {
   public:
      Model(const sptr<Mesh>& mesh, const Matrix4& transform, const Matrix4& worldTransform = Matrix4::Identity)
            : Mesh(mesh), Transforms(), WorldTransform(worldTransform) {
         Transforms.push_back(transform);
         CreateBuffers();
      }

      Model(const sptr<Mesh>& mesh, const std::vector<Matrix4>& transforms, const Matrix4& worldTransform = Matrix4::Identity)
         : Mesh(mesh), Transforms(transforms), WorldTransform(worldTransform) {
         CreateBuffers();
      }

      void CreateBuffers() {
         if (Transforms.empty()) {
            return;
         }
         std::vector<sb::InstanceData> instanceDatas(Transforms.size());
         for (int i = 0; i < Transforms.size(); ++i) {
            instanceDatas[i].model = Transforms[i] * WorldTransform;
            instanceDatas[i].modelNormal = Transpose(Invert(instanceDatas[i].model));
         }

         InstanceData.Create(L"", Transforms.size(), sizeof(sb::InstanceData), instanceDatas.data());
      }

      sptr<Mesh> Mesh;
      std::vector<Matrix4> Transforms;
      Matrix4 WorldTransform;
      StructuredBuffer InstanceData;
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

   float dist = 2.f;
   uint n = 10;
   std::vector<Matrix4> pos(n * n);
   for (int z = 0; z < n; ++z) {
      for (int x = 0; x < n; ++x) {
         pos[n * z + x] = Matrix4::CreateTranslation(x * dist, 0, z * dist);
      }
   }

   s_models.push_back({ Mesh::CreateFromMeshData(GeometryGenerator::CreateGrid(50, 50, 2, 2)), Matrix4::CreateTranslation(0, -1, 0) });

   s_models.push_back({ Mesh::CreateFromMeshData(GeometryGenerator::CreateBox(1, 1, 1, 1)), pos, Matrix4::CreateTranslation(0, 0, 0) });
   s_models.push_back({ Mesh::CreateFromMeshData(GeometryGenerator::CreateCylinder(0.5f, 0.1f, 1, 32, 2)), pos, Matrix4::CreateTranslation(0, 1, 0) });
   s_models.push_back({ Mesh::CreateFromMeshData(GeometryGenerator::CreateGeosphere(0.5f, 3)), pos, Matrix4::CreateTranslation(0, 2, 0) });
   
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
   if (!m_vertexShader["model"] || !m_pixelShader["model"])
   {
      return;
   }

   ColorBuffer& colorBuffer = renderer.GetLDRTarget();
   DepthBuffer& depthBuffer = renderer.GetDepth();

   GraphicsContext& context = GraphicsContext::Begin(L"Scene Render");

   context.TransitionResource(colorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
   context.TransitionResource(depthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
   context.ClearColor(colorBuffer);
   context.ClearDepth(depthBuffer);

   context.SetPipelineState(m_modelPSO["model"]);
   context.SetRootSignature(m_rootSignature["model"]);
   context.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
   context.SetRenderTarget(colorBuffer.GetRTV(), depthBuffer.GetDSV());
   context.SetViewportAndScissor(0, 0, colorBuffer.GetWidth(), colorBuffer.GetHeight());

   CB_ALIGN sb::CBFrame cbFrame;
   cbFrame.g_time = 0.f; // todo:
   cbFrame.eye = m_camera.GetPosition();
   cbFrame.viewProj = m_camera.GetViewProjMatrix();

   CB_ALIGN struct {
      sb::Light light;
   } cbLight;
   cbLight.light = { Vector3(0, 2, 2), Vector3(1, 1, 1) };

   context.SetDynamicConstantBufferView(0, sizeof(cbFrame), &cbFrame);
   context.SetDynamicConstantBufferView(1, sizeof(cbLight), &cbLight);

   for (auto && model : s_models) {
      model.Mesh->SetGeometry(context);
      context.SetBufferSRV(2, model.InstanceData);
      context.DrawIndexedInstanced(model.Mesh->IndexesCount(), model.Transforms.size(), 0, 0, 0);
   }

   context.Finish();
}

void ExampleLayer::OnImGuiRender() 
{
   // ImGui::Begin("Test");
   // ImGui::End();
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

void ExampleLayer::BuildShadersAndPSOForType(const std::string& type) {
   if (!m_vertexShader.count(type)) {
      m_vertexShader[type] = {};
   }
   if (!m_pixelShader.count(type)) {
      m_pixelShader[type] = {};
   }

   auto& vertexShader = m_vertexShader[type];
   auto& pixelShader = m_pixelShader[type];

   {
      std::vector<D3D_SHADER_MACRO> defines;
      auto vShader = Shader::Create("../Shaders/base.hlsl", "baseVS", ShaderType::Vertex, defines.data());
      auto pShader = Shader::Create("../Shaders/base.hlsl", "basePS", ShaderType::Pixel, defines.data());
      if (vShader && pShader) {
         vertexShader = vShader;
         pixelShader = pShader;
      } else {
         return;
      }
   }

   if (!m_rootSignature.count(type)) {
      m_rootSignature[type] = {};
   }
   if (!m_modelPSO.count(type)) {
      m_modelPSO[type] = {};
   }

   auto& rootSignature = m_rootSignature[type];
   auto& pso = m_modelPSO[type];

   rootSignature.Reset(3, 0);
   rootSignature[0].InitAsConstantBuffer(0);
   rootSignature[1].InitAsConstantBuffer(1);
   rootSignature[2].InitAsBufferSRV(0, 1);
   rootSignature.Finalize(L"Triangle", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

   D3D12_INPUT_ELEMENT_DESC vertElem[] =
   {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
   };

   pso.SetRootSignature(rootSignature);
   pso.SetRasterizerState(RasterizerDefault);
   pso.SetBlendState(BlendDisable);
   pso.SetDepthStencilState(DepthStateReadWrite);
   pso.SetInputLayout(_countof(vertElem), vertElem);
   pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
   pso.SetRenderTargetFormat(gn::Application::Get().GetRenderer().GetLDRFormat(), gn::Application::Get().GetRenderer().GetDepthFormat());
   pso.SetVertexShader(vertexShader->GetBytecode());
   pso.SetPixelShader(pixelShader->GetBytecode());
   pso.Finalize();
}

void ExampleLayer::BuildShadersAndPSO() {
   BuildShadersAndPSOForType("model");
}
