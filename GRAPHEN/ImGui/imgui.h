#pragma once

#include "vendors/imgui/imgui.h"

namespace gn
{
   namespace Core
   {
      namespace ImGuiUI
      {
         void Init();
         void NewFrame();
         void Render();
         void Shutdown();

         // use when window resize
         void InvalidateDeviceObjects();
         void CreateDeviceObjects();
      }
   }
}

