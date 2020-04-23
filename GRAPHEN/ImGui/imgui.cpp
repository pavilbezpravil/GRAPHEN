#include "pch.h"

#include "ImGui.h"

#include "VidDriver.h"
#include "SwapChain.h"
#include "Window.h"

#include "vendors/imgui/examples/imgui_impl_win32.h"
#include "vendors/imgui/examples/imgui_impl_dx12.h"


namespace gn
{
   namespace Core
   {
      namespace ImGuiUI
      {
         namespace
         {
            ComPtr<ID3D12DescriptorHeap> g_pd3dSrvDescHeap;
         }

         void Init()
         {
            // Application init: create a dear imgui context, setup some options, load fonts
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.WantCaptureMouse = false;
            // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
            // TODO: Fill optional fields of the io structure later.
            // TODO: Load TTF/OTF fonts if you don't want to use the default font.

            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            ASSERT_SUCCEEDED(Graphics::g_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)));

            // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
            ImGui_ImplWin32_Init(s_Window->GetHWND());
            ImGui_ImplDX12_Init(Graphics::g_Device, s_SwapChain->GetBufferCount(), s_SwapChain->GetFormat(),
               g_pd3dSrvDescHeap.Get(), g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
         }

         void NewFrame()
         {
            // Feed inputs to dear imgui, start new frame
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
         }

         void Render()
         {

            // Render dear imgui into screen
            ImGui::Render();

            GraphicsContext& context = GraphicsContext::Begin(L"ImGui");
            context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_pd3dSrvDescHeap.Get());
            context.SetRenderTarget(s_SwapChain->GetCurrentBackBuffer().GetRTV());
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), context.GetCommandList());
            context.Finish();
         }

         void Shutdown()
         {
            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            g_pd3dSrvDescHeap = nullptr;
         }

         void InvalidateDeviceObjects()
         {
            ImGui_ImplDX12_InvalidateDeviceObjects();
         }

         void CreateDeviceObjects()
         {
            ImGui_ImplDX12_CreateDeviceObjects();
         }
      }
   }
}