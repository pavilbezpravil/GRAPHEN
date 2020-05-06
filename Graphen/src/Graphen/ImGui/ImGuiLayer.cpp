#include "gnpch.h"
#include "Graphen/ImGui/ImGuiLayer.h"
#include <imgui.h>
#include <examples/imgui_impl_dx12.h>
#include <examples/imgui_impl_win32.h>
#include "Graphen/Core/Application.h"
#include "Graphen/Render/VidDriver.h"


namespace gn {

   ImGuiLayer::ImGuiLayer()
      : Layer("ImGuiLayer"), m_descHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, DESC_SIZE), m_nextAllocHandleIdx(2)
   {}

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

      m_descHeap.Create(L"ImGui Descriptor heap");

      m_allocatedDescHandles.resize(DESC_SIZE);
      // m_allocatedDescHandles[0] = m_descHeap.Alloc(1);
      for (int i = 1; i < DESC_SIZE; ++i) {
         m_allocatedDescHandles[i] = m_descHeap.Alloc(1);
      }

      ID3D12DescriptorHeap* srvDescHeap = m_descHeap.GetHeapPointer();
      auto fontHandle = m_descHeap.GetHandleAtOffset(0);
      // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
      ImGui_ImplWin32_Init(hWnd);
      ImGui_ImplDX12_Init(Graphics::g_Device, app.GetWindow().GetSwapChain().GetBufferCount(), app.GetWindow().GetSwapChain().GetFormat(),
         srvDescHeap, fontHandle.GetCpuHandle(), fontHandle.GetGpuHandle());
	}

	void ImGuiLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();

      ImGui_ImplDX12_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
      m_descHeap.DestroyAll();
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
      context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_descHeap.GetHeapPointer());
      context.TransitionResource(app.GetRenderer().GetLDRTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
      context.SetRenderTarget(app.GetRenderer().GetLDRTarget().GetRTV());

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

   void ImGuiLayer::Resize(UINT width, UINT height)
   {
      auto& io = ImGui::GetIO();
      io.DisplaySize.x = (float)width;
      io.DisplaySize.y = (float)height;

      ImGui_ImplDX12_CreateDeviceObjects();
   }

   D3D12_GPU_DESCRIPTOR_HANDLE ImGuiLayer::UploadDescHandle(D3D12_CPU_DESCRIPTOR_HANDLE Handle) {
      uint idx = m_nextAllocHandleIdx;
      m_nextAllocHandleIdx++;
      if (m_nextAllocHandleIdx >= DESC_SIZE) {
         m_nextAllocHandleIdx = 2;
      }
      Graphics::g_Device->CopyDescriptorsSimple(1, m_allocatedDescHandles[idx].GetCpuHandle(), Handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
      return m_allocatedDescHandles[idx].GetGpuHandle();
   }
}
