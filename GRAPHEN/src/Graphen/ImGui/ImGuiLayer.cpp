#include "gnpch.h"
#include "Graphen/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <examples/imgui_impl_dx12.h>
#include <examples/imgui_impl_win32.h>

#include "Graphen/Core/Application.h"
#include "Graphen/Render/VidDriver.h"


namespace gn {

   namespace
   {
      ComPtr<ID3D12DescriptorHeap> g_pd3dSrvDescHeap;
   }

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		HWND hWnd = *static_cast<HWND*>(app.GetWindow().GetNativeWindow());

      D3D12_DESCRIPTOR_HEAP_DESC desc = {};
      desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
      desc.NumDescriptors = 1;
      desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      ASSERT_SUCCEEDED(Graphics::g_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)));

      // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
      ImGui_ImplWin32_Init(hWnd);
      ImGui_ImplDX12_Init(Graphics::g_Device, app.GetWindow().GetSwapChain().GetBufferCount(), app.GetWindow().GetSwapChain().GetFormat(),
         g_pd3dSrvDescHeap.Get(), g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
	}

	void ImGuiLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();

      ImGui_ImplDX12_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
      g_pd3dSrvDescHeap.Reset();
	}
	
	void ImGuiLayer::Begin()
	{
      if (!m_Enable) {
         return;
      }
      HZ_PROFILE_FUNCTION();

      ImGui_ImplDX12_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
      if (!m_Enable) {
         return;
      }

		HZ_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

      GraphicsContext& context = GraphicsContext::Begin(L"ImGui");
      context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_pd3dSrvDescHeap.Get());
      context.SetRenderTarget(app.GetWindow().GetSwapChain().GetCurrentBackBuffer().GetRTV());

      // Rendering
      ImGui::Render();
      ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), context.GetCommandList());

      // Update and Render additional Platform Windows
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
         ImGui::UpdatePlatformWindows();
         ImGui::RenderPlatformWindowsDefault(NULL, (void*)context.GetCommandList());
      }

      context.Finish();
	}

   void ImGuiLayer::InvalidateDeviceObjects()
   {
      ImGui_ImplDX12_InvalidateDeviceObjects();
   }

   void ImGuiLayer::CreateDeviceObjects()
   {
      ImGui_ImplDX12_CreateDeviceObjects();
   }

   void ImGuiLayer::Resize(UINT width, UINT height)
   {
      auto& io = ImGui::GetIO();
      io.DisplaySize.x = width;
      io.DisplaySize.y = height;

      ImGui_ImplDX12_CreateDeviceObjects();
   }
}
