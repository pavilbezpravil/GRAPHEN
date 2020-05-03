#include "ExampleLayer.h"

#include "imgui/imgui.h"
#include "Graphen/Render/CommandContext.h"


using namespace GameCore;
using namespace Graphics;

namespace
{
   struct Vertex {
      Vector3 position;
      Vector3 normal;

      Vertex(const Vector3& position, const Vector3& normal = Vector3::UnitY)
         : position(position),
           normal(normal)
      {
      }
   };

   class Mesh
   {
      std::string m_Name;

      std::vector<uint16_t> indexes;
      std::vector<Vertex> vertex;

      ByteAddressBuffer m_VertexBuffer;
      ByteAddressBuffer m_IndexBuffer;

   public:
      Mesh(const Vertex* vs, int vsSize, const uint16_t* inds, int indsSize, const char* name)
      {
         m_Name = name;
         indexes = { inds, inds + indsSize };
         vertex = { vs, vs + vsSize };

         m_IndexBuffer.Create(L"Indexes", (uint32_t)indexes.size(), sizeof(uint16_t), indexes.data());
         m_VertexBuffer.Create(L"Vertex", (uint32_t)vertex.size(), sizeof(Vertex), vertex.data());
      }

      void SetGeometry(GraphicsContext& context)
      {
         context.SetIndexBuffer(m_IndexBuffer.IndexBufferView());
         context.SetVertexBuffer(0, m_VertexBuffer.VertexBufferView());
      }

      UINT IndexesCount()
      {
         return m_IndexBuffer.GetElementCount();
      }

      static sptr<Mesh> CreateFromVertex(const Vertex* vs, int vsSize, const uint16_t* inds, int indsSize, const char* name = "")
      {
         return gn::CreateRef<Mesh>(vs, vsSize, inds, indsSize, name);
      }

      static sptr<Mesh> CreateFromVertex(const std::vector<Vertex> vs, const std::vector<uint16_t> inds, const char* name = "")
      {
         return Mesh::CreateFromVertex(vs.data(), vs.size(), inds.data(), inds.size(), name);
      }

      static sptr<Mesh> CreateCube(float size = 0.5)
      {
         Vertex vertices[] = {
            Vector3{-size, -size, size},
            Vector3{ size, -size, size},
            Vector3{ -size, size, size},
            Vector3{ size, size, size},

            Vector3{-size, -size, -size},
            Vector3{ size, -size, -size},
            Vector3{ -size, size, -size},
            Vector3{ size, size, -size},
         };

         uint16_t indexes[] = {
            0, 1, 3,
            0, 3, 2,

            5, 4, 7,
            7, 4, 6,

            4, 0, 2,
            4, 2, 6,

            1, 5, 3,
            3, 5, 7,

            2, 3, 7,
            2, 7, 6,

            4, 1, 0,
            4, 5, 1,
         };

         return CreateFromVertex(vertices, _countof(vertices), indexes, _countof(indexes));
      }

      static sptr<Mesh> CreateGrid(const Vector3& normal, const Vector3& tangent, uint16_t n, Vector2 size = Vector2::One)
      {
         std::vector<Vertex> vertices;
         std::vector<uint16_t> indexes;

         const Vector3 bitangent = Cross(normal, tangent);
         const Vector3 lt = -0.5f * size.x * tangent + -0.5f * size.y * bitangent;

         for (int y = 0; y < n; ++y) {
            for (int x = 0; x < n; ++x) {
               Vertex v = { lt + size.x * tangent * (float)x / (float)(n - 1) + size.y * bitangent * (float)y / (float)(n - 1) };
               v.normal = normal;
               vertices.emplace_back(v);
               if (y > 0 && x > 0) {
                  indexes.push_back((y - 1) * n + x - 1);
                  indexes.push_back((y - 1) * n + x    );
                  indexes.push_back((y    ) * n + x    );

                  indexes.push_back((y - 1) * n + x - 1);
                  indexes.push_back((y    ) * n + x    );
                  indexes.push_back((y    ) * n + x - 1);
               }
            }
         }

         return CreateFromVertex(vertices, indexes);
      }

      // static sptr<Mesh> CreateSphereUV(uint16_t uCount, uint16_t vCount, float radius = 0.5f)
      // {
      //    std::vector<Vertex> vertices;
      //    std::vector<uint16_t> indexes;
      //
      //    Vector3 z = Vector3::UnitZ;
      //
      //    for (int u = 0; u < uCount; ++u) {
      //       float phi = XM_2PI / uCount;
      //       for (int v = 1; v < vCount - 1; ++v){
      //          float theta = XM_PIDIV2 - XM_PI / vCount;
      //          Vector3 v = Quaternion(Vector3::UnitY, phi) * Quaternion::Identity
      //       }
      //    }
      //
      //
      //    return CreateFromVertex(vertices, _countof(vertices), indexes, _countof(indexes));
      // }
   };

   class Model
   {
   public:
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

   s_models.push_back({ Mesh::CreateGrid(Vector3::UnitZ, Vector3::UnitX, 5), Matrix4::Identity });
   s_models.push_back({ Mesh::CreateGrid(Vector3::UnitZ, Vector3::UnitX, 2), Matrix4::Identity });
   s_models.back().Transform = Matrix4::CreateRotationX(XMConvertToRadians(-90)) * Matrix4::CreateScale(10) * Matrix4::CreateTranslation(0, -1, 0);
   
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

   ColorBuffer& colorBuffer = renderer.GetColorBufferLDR();
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
	ImGui::Begin("Settings");

	// ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();

   static bool showDemo = true;
   ImGui::ShowDemoWindow(&showDemo);
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
       // { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       // { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       // { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
   };

   m_modelPSO.SetRootSignature(m_rootSignature);
   m_modelPSO.SetRasterizerState(RasterizerDefault);
   m_modelPSO.SetBlendState(BlendDisable);
   m_modelPSO.SetDepthStencilState(DepthStateReadWrite);
   m_modelPSO.SetInputLayout(_countof(vertElem), vertElem);
   m_modelPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
   m_modelPSO.SetRenderTargetFormat(gn::Application::Get().GetRenderer().GetColorLDRFormat(), gn::Application::Get().GetRenderer().GetDepthFormat());
   m_modelPSO.SetVertexShader(m_vertexShader->GetBytecode());
   m_modelPSO.SetPixelShader(m_pixelShader->GetBytecode());
   m_modelPSO.Finalize();
}
