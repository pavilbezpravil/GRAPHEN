#include "gnpch.h"
#include "Graphen/Core/Application.h"

#include "Graphen/Core/Log.h"


#include "Graphen/Core/Input.h"

#include "Graphen/Render/VidDriver.h"
#include "Graphen/Render/GraphicsCommon.h"
#include "Graphen/Render/GpuTimeManager.h"
#include "Graphen/Render/BufferManager.h"
#include "SystemTime.h"

namespace gn {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

      SystemTime::Initialize();

		m_Window = Window::Create();
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));

      VidDriver::Initialize();

      m_Window->InitSwapChain();

      Graphics::InitializeCommonState();

      GpuTimeManager::Initialize(4096);
      Graphics::SetNativeResolution();

      m_Window->Show();

      m_EnableImGui = true;
		m_ImGuiLayer = new ImGuiLayer();
      if (m_EnableImGui) {
         PushOverlay(m_ImGuiLayer);
      }
	}

	Application::~Application()
	{
		HZ_PROFILE_FUNCTION();

      Graphics::g_CommandManager.IdleGPU();

      m_LayerStack.Clear();
      m_ImGuiLayer = nullptr;

      Graphics::g_CommandManager.IdleGPU();

      CommandContext::DestroyAllContexts();
      Graphics::g_CommandManager.Shutdown();
      GpuTimeManager::Shutdown();
      m_Window->ShutdownSwapChain();
      PSO::DestroyAll();
      RootSignature::DestroyAll();
      DescriptorAllocator::DestroyAll();

      Graphics::DestroyCommonState();
      Graphics::DestroyRenderingBuffers();

      s_vidDriver->Shutdown();

      m_Window.reset();
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();

      HZ_CORE_INFO("{0}", e.ToString());

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(Application::OnKeyPressedEvent));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
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

         {
            GraphicsContext& context = GraphicsContext::Begin(L"Backbuffer to RT");
            context.TransitionResource(GetWindow().GetSwapChain().GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
            context.Finish();
         }

         // std::this_thread::sleep_for(std::chrono::milliseconds(25));

         int64_t curFrameTime = timer.GetCurrentTick();

         float time = timer.TimeBetweenTicks(prevFrameTime, curFrameTime);
			Timestep timestep = time;
         prevFrameTime = curFrameTime;

         // HZ_CORE_INFO("Frame time: {0}", time);

			if (!m_Minimized)
			{
				{
					HZ_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

            if (m_EnableImGui)
            {
               m_ImGuiLayer->Begin();
               {
                  HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

                  for (Layer* layer : m_LayerStack)
                     layer->OnImGuiRender();
               }
               m_ImGuiLayer->End();
            }
			}

			m_Window->OnUpdate(timestep);
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

      HZ_CORE_INFO("Changing display resolution to {0}x{1}", width, height);
      m_ImGuiLayer->InvalidateDeviceObjects();
      window.Resize(width, height);
      m_ImGuiLayer->Resize(width, height);

      Graphics::g_CommandManager.IdleGPU();
      Graphics::ResizeDisplayDependentBuffers(width, height);

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
         HZ_CORE_INFO("pressed I");
         m_EnableImGui = !m_EnableImGui;
      }

      return false;
   }

}
