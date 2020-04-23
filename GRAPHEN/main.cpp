#include "pch.h"
#include "Engine.h"
#include "RootSignature.h"
#include "GraphicsCore.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "Shader.h"
#include "Window.h"
#include "SwapChain.h"
#include "CommandContext.h"
#include "Camera.h"
#include "CameraController.h"
#include "VidDriver.h"
#include "GameInput.h"

#include "ImGui/ImGui.h"

using namespace Graphics;
using namespace GameCore;
using namespace gn;

class TestTriangle : public TestApp
{
public:
   void Init() override
   {
      m_RootSignature.Reset(1, 0);
      m_RootSignature[0].InitAsConstantBuffer(0);
      m_RootSignature.Finalize(L"Triangle", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

      // m_depth.Create(L"")
      // m_color.Create(L"SceneColor", g_DisplayHeight, g_DisplayHeight, 1, s_SwapChain->GetFormat());

      D3D12_INPUT_ELEMENT_DESC vertElem[] =
      {
          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
          // { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
          // { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
          // { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
          // { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
      };

      vertexShader = Shader::Create(L"Shaders/base.hlsl", "baseVS", ShaderType::Vertex);
      pixelShader = Shader::Create(L"Shaders/base.hlsl", "basePS", ShaderType::Pixel);
      ASSERT(vertexShader);
      ASSERT(pixelShader);

      m_TrianglePSO.SetRootSignature(m_RootSignature);
      m_TrianglePSO.SetRasterizerState(RasterizerTwoSided);
      m_TrianglePSO.SetBlendState(BlendDisable);
      m_TrianglePSO.SetDepthStencilState(DepthStateDisabled);
      m_TrianglePSO.SetInputLayout(_countof(vertElem), vertElem);
      m_TrianglePSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
      // m_TrianglePSO.SetRenderTargetFormats(1, &m_color.GetFormat(), DXGI_FORMAT_D32_FLOAT);
      m_TrianglePSO.SetRenderTargetFormat(s_SwapChain->GetFormat(), DXGI_FORMAT_UNKNOWN);
      // m_TrianglePSO.SetRenderTargetFormats(1, &m_color.GetFormat(), DXGI_FORMAT_UNKNOWN);
      m_TrianglePSO.SetVertexShader(vertexShader->GetBytecode());
      m_TrianglePSO.SetPixelShader(pixelShader->GetBytecode());
      m_TrianglePSO.Finalize();


      struct Vertex {
         Vector3 position;

         Vertex(const Vector3& position)
            : position(position)
         {
         }
      };

      Vertex pos[] = {
         Vector3{-0.5f, -0.5f, 0.f},
         Vector3{ 0.5f, -0.5f, 0.f},
         Vector3{ 0.0f,  0.5f, 0.f},
      };

      GPU_ALIGN uint16_t indexes[] = {
         0, 1, 2,
         0, 2, 1,
      };

      m_IndexBuffer.Create(L"Indexes", 6, sizeof(uint16_t), indexes);
      m_VertexBuffer.Create(L"Vertex", 3, sizeof(Vertex), pos);

      const Vector3 eye = Vector3(0, 0, 0.f);
      m_Camera.SetEyeAtUp(eye, Vector3::UnitZ, Vector3::UnitY);
      m_Camera.SetZRange(0.1f, 100.0f);
      m_Camera.SetFOV(0.25f * XM_PI);
      m_Camera.SetAspectRatio(1080.f / 1920.f);
      m_CameraController.reset(new CameraController(m_Camera, Vector3::UnitY));

      Core::ImGuiUI::Init();
   }
  

   void ImGui_Draw()
   {
      // Any application code here
      ImGui::Text("Hello, world!");

      // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
      {
         static float f = 0.0f;
         static int counter = 0;

         ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

         ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
         // ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
         // ImGui::Checkbox("Another Window", &show_another_window);

         ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
         // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

         if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
         ImGui::SameLine();
         ImGui::Text("counter = %d", counter);

         ImGui::Text("Mouse position %.3f ms/frame (%.1f FPS)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
         ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

         ImGui::End();
      }
   }

   void Update(float dt) override
   {
      if (GameInput::IsFirstPressed(GameInput::kKey_t)) {
         m_CameraController->Enable(!m_CameraController->Enable());
         s_Window->ShowCursor(!m_CameraController->Enable());
      }

      m_CameraController->Update(dt);
   }

   void ImGui_Render()
   {
      Core::ImGuiUI::NewFrame();
      ImGui_Draw();
      Core::ImGuiUI::Render();
   }

   void Render() override
   {
      GraphicsContext& context = GraphicsContext::Begin(L"Scene Render");

      ColorBuffer& backBuffer = s_SwapChain->GetCurrentBackBuffer();
      backBuffer.SetClearColor(Color(0.5f, 0.5f, 0.f));
      context.ClearColor(backBuffer);

      context.SetPipelineState(m_TrianglePSO);
      context.SetRootSignature(m_RootSignature);
      context.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      context.SetVertexBuffer(0, m_VertexBuffer.VertexBufferView());
      context.SetIndexBuffer(m_IndexBuffer.IndexBufferView(0));
      context.SetRenderTarget(backBuffer.GetRTV());
      context.SetViewportAndScissor(0, 0, g_NativeWidth, g_NativeHeight);

      struct {
         Matrix4 model;
         Matrix4 modelInv;
         Matrix4 projView;
      } cbPerInstance;

      cbPerInstance.model = Matrix4::CreateScale(1.f);
      cbPerInstance.model = Matrix4::CreateTranslation(Vector3{0, 0, 0.5f});
      // cbPerInstance.projView = Matrix4::MakeScale(1.f);
      // cbPerInstance.projView = m_Camera.GetViewProjMatrix();
      cbPerInstance.projView = m_Camera.GetProjMatrix();
      cbPerInstance.projView = m_Camera.GetViewMatrix();
      cbPerInstance.projView = m_Camera.GetViewProjMatrix();
      // cbPerInstance.projView = XMMatrixLookAtLH(Vector3::Zero, Vector3::UnitZ, Vector3::UnitY);
      // cbPerInstance.projView = XMMatrixLookAtRH(Vector3::Zero, Vector3::UnitZ, Vector3::UnitY);
      // cbPerInstance.projView = XMMatrixPerspectiveFovLH(0.25f * XM_PI, 1.33f, 0.1f, 100.f);
      // cbPerInstance.projView = Matrix4::Identity;
      // cbPerInstance.projView = m_Camera.GetViewProjMatrix();

      context.SetDynamicConstantBufferView(0, sizeof(cbPerInstance), &cbPerInstance);

      context.DrawIndexed(3);

      context.Finish();

      ImGui_Render();
   }

   ~TestTriangle()
   {
      Core::ImGuiUI::Shutdown();
   }

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
   uptr<CameraController> m_CameraController;
};

int main()
{
   gn::Engine::Initialize("GRAPHEN");

   auto app = new TestTriangle();
   gn::s_Engine->SetTestApp(app);

   gn::s_Engine->Run();
}
