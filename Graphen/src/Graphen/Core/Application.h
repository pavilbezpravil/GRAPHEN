#pragma once

#include "Graphen/Core/Core.h"

#include "Graphen/Core/Window.h"
#include "Graphen/Core/LayerStack.h"
#include "Graphen/Events/Event.h"
#include "Graphen/Events/ApplicationEvent.h"

#include "Graphen/Core/Timestep.h"

#include "Graphen/ImGui/ImGuiLayer.h"

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

		Window& GetWindow() { return *m_Window; }

		static Application& Get() { return *s_Instance; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
      bool OnKeyPressedEvent(KeyPressedEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
      bool m_EnableImGui;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
   private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

   // To be defined in CLIENT
	Application* CreateApplication();

}