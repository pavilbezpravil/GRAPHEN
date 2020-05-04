#include "gnpch.h"
#include "VidDriver.h"
#include "CommandContext.h"


#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#include <agile.h>
#endif

#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_4.h>    // For WARP
#endif

VidDriver* s_vidDriver = nullptr;

namespace Graphics
{
   ID3D12Device* g_Device = nullptr;
}

namespace Graphics
{
   CommandListManager g_CommandManager;
   ContextManager g_ContextManager;

   D3D_FEATURE_LEVEL g_D3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;

   DescriptorAllocator g_DescriptorAllocator[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
   {
       D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
       D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
       D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
       D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
   };
}

#ifdef ENABLE_EXPERIMENTAL_DXIL_SUPPORT
// A more recent Windows SDK than currently required is needed for these.
typedef HRESULT(WINAPI *D3D12EnableExperimentalFeaturesFn)(
   UINT                                    NumFeatures,
   __in_ecount(NumFeatures) const IID*     pIIDs,
   __in_ecount_opt(NumFeatures) void*      pConfigurationStructs,
   __in_ecount_opt(NumFeatures) UINT*      pConfigurationStructSizes);

static const GUID D3D12ExperimentalShaderModelsID = // 76f5573e-f13a-40f5-b297-81ce9e18933f
{
    0x76f5573e, 0xf13a, 0x40f5, { 0xb2, 0x97, 0x81, 0xce, 0x9e, 0x18, 0x93, 0x3f }
};

using namespace DirectX;

static HRESULT EnableExperimentalShaderModels()
{
   HMODULE hRuntime = LoadLibraryW(L"d3d12.dll");
   if (!hRuntime)
      return HRESULT_FROM_WIN32(GetLastError());

   D3D12EnableExperimentalFeaturesFn pD3D12EnableExperimentalFeatures =
      (D3D12EnableExperimentalFeaturesFn)GetProcAddress(hRuntime, "D3D12EnableExperimentalFeatures");

   if (pD3D12EnableExperimentalFeatures == nullptr)
   {
      FreeLibrary(hRuntime);
      return HRESULT_FROM_WIN32(GetLastError());
   }

   return pD3D12EnableExperimentalFeatures(1, &D3D12ExperimentalShaderModelsID, nullptr, nullptr);
}
#else
static HRESULT EnableExperimentalShaderModels() { return S_OK; }
#endif

void VidDriver::Initialize()
{
   ASSERT(s_vidDriver == nullptr, "Graphics has already been initialized");
   s_vidDriver = new VidDriver();

   Microsoft::WRL::ComPtr<ID3D12Device> pDevice;

#if _DEBUG
   Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
   if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {
      debugInterface->EnableDebugLayer();
   } else {
      GN_CORE_WARN("Unable to enable D3D12 debug validation layer");
   }
#endif

   EnableExperimentalShaderModels();

   // Obtain the DXGI factory
   Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
   ASSERT_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

   // Create the D3D graphics device
   Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;

   static const bool bUseWarpDriver = false;

   if (!bUseWarpDriver)
   {
      SIZE_T MaxSize = 0;

      for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(Idx, &pAdapter); ++Idx)
      {
         DXGI_ADAPTER_DESC1 desc;
         pAdapter->GetDesc1(&desc);
         if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

         if (desc.DedicatedVideoMemory > MaxSize && SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
         {
            pAdapter->GetDesc1(&desc);
            GN_CORE_INFO("D3D12-capable hardware found:  {} ({} MB)\n", MakeStr(desc.Description), desc.DedicatedVideoMemory >> 20);
            MaxSize = desc.DedicatedVideoMemory;
         }
      }

      if (MaxSize > 0)
         Graphics::g_Device = pDevice.Detach();
   }

   if (Graphics::g_Device == nullptr)
   {
      if (bUseWarpDriver) {
         GN_CORE_WARN("WARP software adapter requested.  Initializing...");
      } else {
         GN_CORE_WARN("Failed to find a hardware adapter.  Falling back to WARP");
      }
      ASSERT_SUCCEEDED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter)));
      ASSERT_SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice)));
      Graphics::g_Device = pDevice.Detach();
   }
#ifndef RELEASE
   else
   {
      bool DeveloperModeEnabled = false;

      // Look in the Windows Registry to determine if Developer Mode is enabled
      HKEY hKey;
      LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
      if (result == ERROR_SUCCESS)
      {
         DWORD keyValue, keySize = sizeof(DWORD);
         result = RegQueryValueEx(hKey, L"AllowDevelopmentWithoutDevLicense", 0, NULL, (byte*)&keyValue, &keySize);
         if (result == ERROR_SUCCESS && keyValue == 1)
            DeveloperModeEnabled = true;
         RegCloseKey(hKey);
      }

      WARN_ONCE_IF_NOT(DeveloperModeEnabled, "Enable Developer Mode on Windows 10 to get consistent profiling results");

      // Prevent the GPU from overclocking or underclocking to get consistent timings
      if (DeveloperModeEnabled)
         Graphics::g_Device->SetStablePowerState(TRUE);
   }
#endif    

#if _DEBUG
   ID3D12InfoQueue* pInfoQueue = nullptr;
   if (SUCCEEDED(Graphics::g_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
   {
      // Suppress whole categories of messages
      //D3D12_MESSAGE_CATEGORY Categories[] = {};

      // Suppress messages based on their severity level
      D3D12_MESSAGE_SEVERITY Severities[] =
      {
          D3D12_MESSAGE_SEVERITY_INFO
      };

      // Suppress individual messages by their ID
      D3D12_MESSAGE_ID DenyIds[] =
      {
         // This occurs when there are uninitialized descriptors in a descriptor table, even when a
         // shader does not access the missing descriptors.  I find this is common when switching
         // shader permutations and not wanting to change much code to reorder resources.
         D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

         // Triggered when a shader does not export all color components of a render target, such as
         // when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
         D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

         // This occurs when a descriptor table is unbound even when a shader does not access the missing
         // descriptors.  This is common with a root signature shared between disparate shaders that
         // don't all need the same types of resources.
         D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

         // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
         (D3D12_MESSAGE_ID)1008,
      };

      D3D12_INFO_QUEUE_FILTER NewFilter = {};
      //NewFilter.DenyList.NumCategories = _countof(Categories);
      //NewFilter.DenyList.pCategoryList = Categories;
      NewFilter.DenyList.NumSeverities = _countof(Severities);
      NewFilter.DenyList.pSeverityList = Severities;
      NewFilter.DenyList.NumIDs = _countof(DenyIds);
      NewFilter.DenyList.pIDList = DenyIds;

      pInfoQueue->PushStorageFilter(&NewFilter);
      pInfoQueue->Release();
   }
#endif

   // We like to do read-modify-write operations on UAVs during post processing.  To support that, we
   // need to either have the hardware do typed UAV loads of R11G11B10_FLOAT or we need to manually
   // decode an R32_UINT representation of the same buffer.  This code determines if we get the hardware
   // load support.
   D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData = {};
   if (SUCCEEDED(Graphics::g_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData))))
   {
      if (FeatureData.TypedUAVLoadAdditionalFormats)
      {
         D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
         {
             DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
         };

         if (SUCCEEDED(Graphics::g_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
            (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
         {
            s_vidDriver->g_bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
         }

         Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

         if (SUCCEEDED(Graphics::g_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
            (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
         {
            s_vidDriver->g_bTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
         }
      }
   }

   Graphics::g_CommandManager.Create(Graphics::g_Device);
}

void VidDriver::Shutdown()
{
   ReportLiveDeviceObjects();

   SafeRelease(Graphics::g_Device);
}

void VidDriver::ReportLiveDeviceObjects()
{
#if defined(_DEBUG)
   ID3D12DebugDevice* debugInterface;
   if (SUCCEEDED(Graphics::g_Device->QueryInterface(&debugInterface)))
   {
      // debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY);
      debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
      debugInterface->Release();
   }
#endif
}
