#pragma once

#include "TestApp.h"

namespace gn
{

   class Engine
   {
   public:
      void Run();

      static void Initialize(const char* name);

      // todo: temp
      void SetTestApp(TestApp* app) { this->app = app; app->Init(); }

   private:
      Engine() {};
      ~Engine() {};

      void Terminate();

      bool Update();
      void Render();

      TestApp* app = nullptr;
   };

   extern Engine* s_Engine;

}