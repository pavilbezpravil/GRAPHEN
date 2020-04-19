#include "pch.h"
#include "Engine.h"
#include "BufferManager.h"
#include "SystemTime.h"
#include "GameInput.h"
#include "Window.h"
#include "GpuTimeManager.h"
#include "SwapChain.h"
#include "VidDriver.h"

Engine* s_Engine = nullptr;

void Engine::Run()
{
   do
   {
      MSG msg = {};
      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
      if (msg.message == WM_QUIT)
         break;
   }
   while (Update());

   Graphics::g_CommandManager.IdleGPU();
   SafeDelete(app);

   Terminate();
}

void Engine::Initialize(const char* name)
{
   //ASSERT_SUCCEEDED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED));
   // Microsoft::WRL::Wrappers::RoInitializeWrapper InitializeWinRT(RO_INIT_MULTITHREADED);
   // ASSERT_SUCCEEDED(InitializeWinRT);

   VidDriver::Initialize();
   Window::Initialize(name, 1920, 1080);
   SwapChain::Initialize(g_DisplayWidth, g_DisplayHeight, DXGI_FORMAT_R10G10B10A2_UNORM);

   Graphics::InitializeCommonState();

   GpuTimeManager::Initialize(4096);
   s_Window->SetNativeResolution();

   SystemTime::Initialize();
   GameInput::Initialize();
   EngineTuning::Initialize();
   s_Window->ShowWindow(SW_SHOWDEFAULT);

   s_Engine = new Engine();
}

void Engine::Terminate()
{
   Graphics::g_CommandManager.IdleGPU();

   SwapChain::Terminate();

   GameInput::Shutdown();

   CommandContext::DestroyAllContexts();
   Graphics::g_CommandManager.Shutdown();
   GpuTimeManager::Shutdown();
   SwapChain::Shutdown();
   PSO::DestroyAll();
   RootSignature::DestroyAll();
   DescriptorAllocator::DestroyAll();

   Graphics::DestroyCommonState();
   Graphics::DestroyRenderingBuffers();
   TextureManager::Shutdown();

   Window::Terminate();

   VidDriver::Shutdown();

   SafeDelete(s_Engine);
}

bool Engine::Update()
{
   EngineProfiling::Update();

   float DeltaTime = Graphics::GetFrameTime();

   GameInput::Update(DeltaTime);
   EngineTuning::Update(DeltaTime);

   ASSERT(app);
   app->Update(DeltaTime);
   GraphicsContext& context = GraphicsContext::Begin(L"Backbuffer to RT");
   context.TransitionResource(s_SwapChain->GetBuffer(Graphics::GetBackBufferIndex()), D3D12_RESOURCE_STATE_RENDER_TARGET, true);
   context.Finish();
   app->Render();

   Render();

   return !GameInput::IsFirstPressed(GameInput::kKey_escape);
}

void Engine::Render()
{
   Graphics::Present();
}
