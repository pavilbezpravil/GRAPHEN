#pragma once

#include "Graphen/Core/Window.h"
#include "Graphen/Core/LayerStack.h"
#include "Graphen/Events/ApplicationEvent.h"
#include "Graphen/Render/Renderer.h"
#include "Graphen/ImGui/ImGuiLayer.h"
#include "Graphen/Events/KeyEvent.h"
#include "Graphen/Render/DescriptorHeap.h"
int main(int argc, char** argv);

namespace gn {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_window; }
		Renderer& GetRenderer() { return m_renderer; }

		static Application& Get() { return *s_Instance; }
	private:
		void Run();
      void ImGuiCreateDockspace();
      void ImGuiRenderApplicationDockspace();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
      bool OnKeyPressedEvent(KeyPressedEvent& e);
	private:
		std::unique_ptr<Window> m_window;
      bool m_enableImGui;
		ImGuiLayer* m_imGuiLayer;
		bool m_Running = true;
		bool m_minimized = false;
		LayerStack m_layerStack;

      Renderer m_renderer;

      DescriptorHandle m_texForViewportDescriptorHandle;

   private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

   // To be defined in CLIENT
	Application* CreateApplication();

}