//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard 
//

#include "gnpch.h"
#include "BufferManager.h"
#include "GraphicsCore.h"
#include "CommandContext.h"
#include "EsramAllocator.h"

namespace Graphics
{
    DepthBuffer g_SceneDepthBuffer;
    ColorBuffer g_SceneColorBuffer;
}

#define T2X_COLOR_FORMAT DXGI_FORMAT_R10G10B10A2_UNORM
#define HDR_MOTION_FORMAT DXGI_FORMAT_R16G16B16A16_FLOAT
#define DSV_FORMAT DXGI_FORMAT_D32_FLOAT

void Graphics::InitializeRenderingBuffers( uint32_t bufferWidth, uint32_t bufferHeight )
{
    // GraphicsContext& InitContext = GraphicsContext::Begin();
    EsramAllocator esram;

    esram.PushStack();

        // g_SceneColorBuffer.Create( L"Main Color Buffer", bufferWidth, bufferHeight, 1, DefaultHdrColorFormat, esram );
        g_SceneDepthBuffer.Create(L"Scene Depth Buffer", bufferWidth, bufferHeight, DSV_FORMAT, esram);

    esram.PopStack(); // End final image

    // InitContext.Finish();
}

void Graphics::ResizeDisplayDependentBuffers(uint32_t /*NativeWidth*/, uint32_t NativeHeight)
{
    
}

void Graphics::DestroyRenderingBuffers()
{
    g_SceneDepthBuffer.Destroy();
    g_SceneColorBuffer.Destroy();
}
