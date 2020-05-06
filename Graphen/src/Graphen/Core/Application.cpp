#include "gnpch.h"
#include "Graphen/Core/Application.h"
#include "Graphen/Core/Log.h"
#include "Graphen/Core/Input.h"
#include "Graphen/Render/VidDriver.h"
#include "Graphen/Render/GraphicsCommon.h"
#include "Graphen/Render/GpuTimeManager.h"
#include "SystemTime.h"
#include "Graphen/Render/RenderUtils.h"
#include <imgui.h>

namespace gn {

   Application* Application::s_Instance = nullptr;

   Application::Application()
   {
      HZ_PROFILE_FUNCTION();

      GN_CORE_ASSERT_MSG(!s_Instance, "Application already exists!");
      s_Instance = this;

      SystemTime::Initialize();

      m_window = Window::Create();
      m_window->SetEventCallback(GN_BIND_EVENT_FN(Application::OnEvent));

      VidDriver::Initialize();

      m_window->InitSwapChain();

      Graphics::InitializeCommonState();

      GpuTimeManager::Initialize(4096);

      m_window->Show();

      m_renderer.Init(m_window->GetWidth(), m_window->GetHeight());
      RenderUtils::Initialize();

      m_enableImGui = true;
		m_imGuiLayer = new ImGuiLayer();
      PushOverlay(m_imGuiLayer);
	}

	Application::~Application()
	{
		HZ_PROFILE_FUNCTION();

      Graphics::g_CommandManager.IdleGPU();

      m_layerStack.Clear();
      m_imGuiLayer = nullptr;

      Graphics::g_CommandManager.IdleGPU();

      m_renderer.Destroy();
      RenderUtils::Shutdown();

      CommandContext::DestroyAllContexts();
      Graphics::g_CommandManager.Shutdown();
      GpuTimeManager::Shutdown();
      m_window->ShutdownSwapChain();
      PSO::DestroyAll();
      RootSignature::DestroyAll();
      DescriptorAllocator::DestroyAll();

      Graphics::DestroyCommonState();

      s_vidDriver->Shutdown();

      m_window.reset();
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_layerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();

      // GN_CORE_INFO("{0}", e.ToString());

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(GN_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(GN_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<KeyPressedEvent>(GN_BIND_EVENT_FN(Application::OnKeyPressedEvent));
      dispatcher.Dispatch<AppKillFocusEvent>([&] (AppKillFocusEvent& _e) { GetWindow().ShowCursor(true); return false; });

		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		HZ_PROFILE_FUNCTION();

      SystemTime timer;
      int64_t prevFrameTime = timer.GetCurrentTick();

      m_window->ShowCursor(false);

		while (m_Running)
		{
			HZ_PROFILE_SCOPE("RunLoop");

         m_window->OnHandleInput();

         // std::this_thread::sleep_for(std::chrono::milliseconds(25));

         int64_t curFrameTime = timer.GetCurrentTick();
         float time = (float)timer.TimeBetweenTicks(prevFrameTime, curFrameTime);
			Timestep timestep = time;
         prevFrameTime = curFrameTime;

         Input::Update(timestep.GetSeconds());

         // GN_CORE_INFO("Frame time: {0}", time);

			if (!m_minimized)
			{
				{
					HZ_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_layerStack)
						layer->OnUpdate(timestep);
				}

            {
               HZ_PROFILE_SCOPE("LayerStack OnRender");
            
               for (Layer* layer : m_layerStack)
                  layer->OnRender(m_renderer);
            }

            m_renderer.SwapLDRBuffer();

            if (m_enableImGui)
            {
               m_imGuiLayer->Begin();
               {
                  HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

                  ImGuiCreateDockspace();
                  for (Layer* layer : m_layerStack) {
                     layer->OnImGuiRender();
                  }
               }
               m_imGuiLayer->End();
            }
			}

         m_renderer.Present();
		}
	}

   void Application::ImGuiCreateDockspace() {
      static bool open = true;

      static bool opt_fullscreen_persistant = true;
      bool opt_fullscreen = opt_fullscreen_persistant;
      static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

      // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
      // because it would be confusing to have two docking targets within each others.
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
      if (opt_fullscreen) {
         ImGuiViewport* viewport = ImGui::GetMainViewport();
         ImGui::SetNextWindowPos(viewport->GetWorkPos());
         ImGui::SetNextWindowSize(viewport->GetWorkSize());
         ImGui::SetNextWindowViewport(viewport->ID);
         ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
         ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
         window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
         window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
      }

      // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
      // and handle the pass-thru hole, so we ask Begin() to not render a background.
      if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
         window_flags |= ImGuiWindowFlags_NoBackground;

      // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
      // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
      // all active windows docked into it will lose their parent and become undocked.
      // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
      // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("Graphen", &open, window_flags);
      ImGui::PopStyleVar();

      if (opt_fullscreen)
         ImGui::PopStyleVar(2);

      // DockSpace
      ImGuiIO& io = ImGui::GetIO();
      if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
         ImGuiID dockspace_id = ImGui::GetID("Graphen Docspace");
         ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
      } else {
         // ShowDockingDisabledMessage();
      }

      ImGui::End();

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("Viewport");
      ImGui::PopStyleVar();
      auto wSize = ImGui::GetWindowSize();
      ImGui::Image(reinterpret_cast<ImTextureID>(m_imGuiLayer->UploadDescHandle(m_renderer.GetLDRBB().GetSRV()).ptr), { wSize.x, wSize.y });
      // ImGui::Image(reinterpret_cast<ImTextureID>(m_texForViewportDescriptorHandle.GetGpuHandle().ptr), { wSize.x, wSize.y });
      ImGui::End();

      ImGui::Begin("Engine");
      ImGui::Text("Frame %.3f ms/frame\n(%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

      static bool showShadowBuffer = false;
      if (ImGui::Button("Show shadow buffer")) {
         showShadowBuffer = true;
      }
      ImGui::End();

      if (showShadowBuffer) {
         ImGui::Begin("Shadow Buffer");
         auto wSize = ImGui::GetWindowSize();
         ImGui::Image(reinterpret_cast<ImTextureID>(m_imGuiLayer->UploadDescHandle(m_renderer.GetShadow().GetSRV()).ptr), { wSize.x, wSize.y });
         ImGui::End();
      }
   }

   bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();

      auto& window = GetWindow();
      UINT prevWidth = window.GetWidth();
      UINT prevHeight = window.GetHeight();

      UINT width = e.GetWidth();
      UINT height = e.GetHeight();

		if (width == 0 || height == 0)
		{
			m_minimized = true;
			return false;
		}

		m_minimized = false;

      if (width == prevWidth && height == prevHeight)
      {
         return false;
      }

      GN_CORE_INFO("Changing display resolution to {0}x{1}", width, height);
      Graphics::g_CommandManager.IdleGPU();
      m_imGuiLayer->InvalidateDeviceObjects();
      window.Resize(width, height);
      m_renderer.Resize(width, height);
      m_imGuiLayer->Resize(width, height);

      return false;
	}


   bool Application::OnKeyPressedEvent(KeyPressedEvent& e)
   {
      if (e.GetKeyCode() == HZ_KEY_ESCAPE)
      {
         m_Running = !m_Running;
         return true;
      }

      
      if (e.GetKeyCode() == HZ_KEY_I /*&& Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL)*/)
      {
         GN_CORE_INFO("pressed I");
         m_enableImGui = !m_enableImGui;
      }

      return false;
   }

}
