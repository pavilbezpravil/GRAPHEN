#include "ExampleLayer.h"

#include "imgui/imgui.h"
#include "Graphen/Render/CommandContext.h"
#include "Graphen/Render/GeometryGenerator.h"
#include "Graphen/Render/SamplerManager.h"
#include "Graphen/Render/VidDriver.h"
using namespace gn;
using namespace GameCore;
using namespace Graphics;


ExampleLayer::ExampleLayer() 
	: Layer("ExampleLayer") {}

ExampleLayer::~ExampleLayer() {}

void ExampleLayer::OnAttach() {
   m_effect = std::make_shared<Effect>();
   m_effectCloth = std::make_shared<Effect>();
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

   m_scene.AddModel(Model::Create( StaticMesh::CreateFromMeshData(GeometryGenerator::CreateGrid(50, 50, 2, 2)), m_effect,
      Matrix4::CreateTranslation(0, 0, 0) ));

   // m_scene.AddModel(std::make_shared<Model>(StaticMesh::CreateFromMeshData(GeometryGenerator::CreateBox(1, 1, 1, 1)), m_effect,
   //    pos, Matrix4::CreateTranslation(0, 1, 0) ));
   // m_scene.AddModel(std::make_shared<Model>(StaticMesh::CreateFromMeshData(GeometryGenerator::CreateCylinder(0.5f, 0.1f, 1, 32, 2)), m_effect,
   //    pos, Matrix4::CreateTranslation(0, 2, 0) ));
   // m_scene.AddModel(std::make_shared<Model>(StaticMesh::CreateFromMeshData(GeometryGenerator::CreateGeosphere(0.5f, 3)), m_effect,
   //    pos, Matrix4::CreateTranslation(0, 3, 0) ));

   static uint clothN = 8;

   {
      float dist = 4.f;
      uint n = 8;
      m_clothModels.resize(n * n);
      for (uint z = 0; z < n; ++z) {
         for (uint x = 0; x < n; ++x) {
            auto clothTransform = Matrix4::CreateRotationX(XMConvertToRadians(-90.f)) * Matrix4::CreateScale(3, 3, 3) * Matrix4::CreateTranslation(0, 2, 0) * Matrix4::CreateTranslation(x * dist, 0, z * dist);
            auto clothMesh = ClothMesh::Create(clothN, clothN, clothTransform);
            m_clothModels[z * n + x] = Model::Create(clothMesh, m_effectCloth, Matrix4::Identity, clothTransform);
            clothMesh->InitConstrainsBuffer(1);
            m_clothSimulation.AddSimCloth(clothMesh);

            m_clothSpheres.push_back(Model::Create(StaticMesh::CreateFromMeshData(GeometryGenerator::CreateGeosphere(0.5f, 3)), m_effect, Matrix4::CreateTranslation(0, 0.f, 0),
               Matrix4::CreateTranslation(0, 1.5, 0) * Matrix4::CreateTranslation(x * dist, 0, z * dist)));
         }
      }
   }

   for (auto& clothModel : m_clothModels) {
      m_scene.AddModel(clothModel);
   }

   // m_clothSpheres.push_back(Model::Create(StaticMesh::CreateFromMeshData(GeometryGenerator::CreateGeosphere(0.5f, 3)), m_effect, Matrix4::CreateTranslation(0, 0, 0),
   //    Matrix4::CreateTranslation(0, 1.5f, 0)));
   for (auto& s : m_clothSpheres) {
      m_scene.AddModel(s);
   }

   m_clothSimulation.Init();

   const Vector3 eye = Vector3(-2, 1, -3.f);
   m_camera.SetEyeAtUp(eye, eye + Vector3::UnitZ, Vector3::UnitY);
   m_camera.SetZRange(0.1f, 100.0f);
   m_camera.SetFOV(0.25f * XM_PI);
   m_camera.SetAspectRatio(1080.f / 1920.f);
   m_cameraController.reset(new CameraController(m_camera, Vector3::UnitY));
}

void ExampleLayer::OnDetach() {
}

void ExampleLayer::OnUpdate(gn::Timestep ts) {
   m_cameraController->Update(ts.GetSeconds());

   if (m_enableExternalObjects) {
      static float simTime = 0;
      float prevTime = simTime;
      simTime += ts.GetSeconds() * 2 * m_clothSimulation.m_deltaRimeMultiplier;

      for (int i = 0; i < m_clothSpheres.size(); ++i) {
         auto& s = m_clothSpheres[i];
         if (!s->GetEnable()) {
            continue;
         }

         // float temp = float(std::hash<int>()(i) / 100) / 1000.f;
         // temp = temp - int(temp);
         float temp = ((i * 1734987 + 346) % 20) / 10.f + 0.3f;
         Matrix4 t = Matrix4::CreateTranslation(0, 0, 2 * (Sin(simTime * temp) - Sin(prevTime * temp)));
         s->SetWorldMatrix(s->GetWorldMatrix() * t);

         ClothConstraint::SphereCollisionConstraint c{ s->GetWorldMatrix().Translation(), 0.5f };
         auto* clothMesh = dynamic_cast<ClothMesh*>(&m_clothModels[i]->GetMesh());
         if (clothMesh) {
            clothMesh->SetConstrains({c});
         }
      }
   }

   ComputeContext& context = ComputeContext::Begin(L"Cloth Update");
   m_clothSimulation.Update(context, ts);
   context.Finish();
}

void ExampleLayer::OnRender(gn::Renderer& renderer) {
   if (!m_effect || !*m_effect) {
      return;
   }

   renderer.DrawScene(m_scene, m_camera);
}

void ExampleLayer::OnImGuiRender() {
   // static float nearZ;
   // static float farZ;
   //
   // ImGui::Begin("Camera");
   // ImGui::SliderFloat("near z", &nearZ, 0, 10, "%.3f", 2);
   // ImGui::SliderFloat("far z", &farZ, 11, 100, "%.3f", 2);
   // ImGui::End();
   //
   // m_camera.SetZRange(nearZ, farZ);

   static int clothNSize = 32;

   ImGui::Begin("Cloth");
   ImGui::SliderFloat("ks", &m_clothSimulation.gKs, 0, 1, "%.3f", 2);
   ImGui::Checkbox("diagonal", &m_clothSimulation.gUseDiagonal);
   ImGui::SliderFloat("ks diagonal", &m_clothSimulation.gKs_diagonal, 0, 1, "%.3f", 2);
   ImGui::Checkbox("bend", &m_clothSimulation.gUseBend);
   ImGui::SliderFloat("ks bend", &m_clothSimulation.gKs_bend, 0, 1, "%.3f", 2);
   ImGui::SliderFloat("velocity dump", &m_clothSimulation.gKVelocityDump, 0, 1);
   ImGui::SliderInt("iter", &m_clothSimulation.m_iter, 1, 50);
   ImGui::SliderFloat("delta time multiplier", &m_clothSimulation.m_deltaRimeMultiplier, 0.001f, 1.f, "%.3f", 3);
   ImGui::Checkbox("solve pass", &m_clothSimulation.m_solvePass);
   ImGui::SliderInt("n devide", &clothNSize, 4, 32);
   if (ImGui::Checkbox("external objs", &m_enableExternalObjects)) {
      for (auto& s : m_clothSpheres) {
         s->SetEnable(m_enableExternalObjects);
      }
   }
   if (ImGui::Button("Recreate")) {
      Graphics::g_CommandManager.IdleGPU();
      for (auto& clothModel: m_clothModels) {
         auto* clothMesh = dynamic_cast<ClothMesh*>(&clothModel->GetMesh());
         if (clothMesh) {
            clothMesh->RebuildMesh(clothNSize, clothNSize, true);
         }
      }
   }

   ImGui::End();


   for (auto& clothModel : m_clothModels) {
      auto* clothMesh = dynamic_cast<ClothMesh*>(&clothModel->GetMesh());
      if (clothMesh) {
         if (clothNSize != clothMesh->GetN()) {
            Graphics::g_CommandManager.IdleGPU();
            clothMesh->RebuildMesh(clothNSize, clothNSize);
         }
      }
   }
}

void ExampleLayer::OnEvent(gn::Event& e) {
   gn::EventDispatcher dispatcher(e);
   dispatcher.Dispatch<gn::KeyPressedEvent>([&] (gn::KeyPressedEvent& e) {
      if (e.GetKeyCode() == HZ_KEY_T) {
         ToggleCameraControl();
      } else if (e.GetKeyCode() == HZ_KEY_R) {
         BuildShadersAndPSO();
         m_clothSimulation.RebuildShaderAndPSO();
      } else if (e.GetKeyCode() == HZ_KEY_X) {
         m_wireframe = !m_wireframe;
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
      m_camera.SetAspectRatio((float)e.GetHeight() / (float)e.GetWidth());
      return false;
   });
}

void ExampleLayer::ToggleCameraControl() {
   m_cameraController->Enable(!m_cameraController->Enable());
   gn::Application::Get().GetWindow().ShowCursor(!m_cameraController->Enable());
}

void ExampleLayer::BuildShadersAndPSOForPass(gn::EffectRef& effect, const std::string& type, const std::vector<D3D_SHADER_MACRO>& macros) {
   if (!effect->m_vertexShader.count(type)) {
      effect->m_vertexShader[type] = {};
   }
   if (!effect->m_pixelShader.count(type)) {
      effect->m_pixelShader[type] = {};
   }

   auto& vertexShader = effect->m_vertexShader[type];
   auto& pixelShader = effect->m_pixelShader[type];

   {
      std::vector<D3D_SHADER_MACRO> defines;
      if (type == PASS_NAME_Z_PASS) {
         defines.push_back({"Z_PASS", nullptr});
      } else if (type == PASS_NAME_PRERECORD) {
         defines.push_back({ "PRERECORD", nullptr });
      }
      for (auto& macro : macros) {
         defines.push_back(macro);
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

   if (!effect->m_rootSignature.count(type)) {
      effect->m_rootSignature[type] = CreateRef<RootSignature>();

      auto& rootSignature = *effect->m_rootSignature[type];
      rootSignature.Reset(4, 1);
      rootSignature.InitStaticSampler(0, SamplerPointBorderDesc);
      rootSignature[0].InitAsConstantBuffer(0);
      rootSignature[1].InitAsConstantBuffer(1);
      rootSignature[2].InitAsBufferSRV(0, 1);
      rootSignature[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1); // shadow map
      rootSignature.Finalize(L"Model", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
   }
   if (!effect->m_modelPSO.count(type)) {
      effect->m_modelPSO[type] = CreateRef<GraphicsPSO>();
   }

   auto& pso = *effect->m_modelPSO[type];

   D3D12_INPUT_ELEMENT_DESC vertElem[] =
   {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
       { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
   };

   D3D12_DEPTH_STENCIL_DESC depthState = DepthStateReadWrite;
   if (type == PASS_NAME_OPAQUE) {
      depthState = DepthStateTestEqual;
   }


   pso.SetRootSignature(*effect->m_rootSignature[type]);
   // pso.SetRasterizerState(RasterizerDefault); // todo:
   pso.SetRasterizerState(m_wireframe ? RasterizerWireframeTwoSided : RasterizerTwoSided);
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
   if (type == PASS_NAME_PRERECORD) {
      pso.SetRenderTargetFormat(Application::Get().GetRenderer().GetNormal().GetFormat(), Application::Get().GetRenderer().GetDepthFormat());
   }
   pso.SetVertexShader(vertexShader->GetBytecode());
   pso.SetPixelShader(pixelShader->GetBytecode());
   pso.Finalize();
}

void ExampleLayer::BuildShadersAndPSO() {
   BuildShadersAndPSOForPass(m_effect, PASS_NAME_Z_PASS);
   BuildShadersAndPSOForPass(m_effect, PASS_NAME_PRERECORD);
   BuildShadersAndPSOForPass(m_effect, PASS_NAME_OPAQUE);

   std::vector<D3D_SHADER_MACRO> macros = { D3D_SHADER_MACRO{"POS_NORMAL_IN_WORLD_SPACE", 0} };
   BuildShadersAndPSOForPass(m_effectCloth, PASS_NAME_Z_PASS, macros);
   BuildShadersAndPSOForPass(m_effectCloth, PASS_NAME_PRERECORD, macros);
   BuildShadersAndPSOForPass(m_effectCloth, PASS_NAME_OPAQUE, macros);
}
