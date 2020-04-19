#pragma once
#include "CommandContext.h"

class VidDriver
{
public:
   static void Initialize();
   static void Shutdown();
   static void Terminate();

   bool g_bTypedUAVLoadSupport_R11G11B10_FLOAT = false;
   bool g_bTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;

private:


};

namespace Graphics
{
   extern CommandListManager g_CommandManager;
   extern ContextManager g_ContextManager;

   extern D3D_FEATURE_LEVEL g_D3DFeatureLevel;

   extern DescriptorAllocator g_DescriptorAllocator[];
   inline D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT Count = 1)
   {
      return g_DescriptorAllocator[Type].Allocate(Count);
   }

   extern ID3D12Device* g_Device;
}

extern VidDriver* s_vidDriver;
