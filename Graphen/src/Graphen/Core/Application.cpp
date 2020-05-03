#include "gnpch.h"
#include "Graphen/Core/Application.h"

#include "Graphen/Core/Log.h"


#include "Graphen/Core/Input.h"

#include "Graphen/Render/VidDriver.h"
#include "Graphen/Render/GraphicsCommon.h"
#include "Graphen/Render/GpuTimeManager.h"
#include "SystemTime.h"
#include "Graphen/Render/RenderUtils.h"

namespace gn {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_PROFILE_FUNCTION();

		GN_CORE_ASSERT(!s_Instance, "Application already exists!");
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

      m_enableImGui = false;
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

		while (m_Running)
		{
			HZ_PROFILE_SCOPE("RunLoop");

         // std::this_thread::sleep_for(std::chrono::milliseconds(25));

         int64_t curFrameTime = timer.GetCurrentTick();

         float time = timer.TimeBetweenTicks(prevFrameTime, curFrameTime);
			Timestep timestep = time;
         prevFrameTime = curFrameTime;

         // GN_CORE_INFO("Frame time: {0}", time);

			if (!m_Minimized)
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

            if (m_enableImGui)
            {
               m_imGuiLayer->Begin();
               {
                  HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

                  for (Layer* layer : m_layerStack)
                     layer->OnImGuiRender();
               }
               m_imGuiLayer->End();
            }
			}

         m_renderer.Present();
			m_window->OnUpdate(timestep);
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
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

      if (width == prevWidth && height == prevHeight)
      {
         return false;
      }

      Graphics::g_CommandManager.IdleGPU();

      GN_CORE_INFO("Changing display resolution to {0}x{1}", width, height);
      m_imGuiLayer->InvalidateDeviceObjects();
      window.Resize(width, height);
      m_imGuiLayer->Resize(width, height);

      Graphics::g_CommandManager.IdleGPU();

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
