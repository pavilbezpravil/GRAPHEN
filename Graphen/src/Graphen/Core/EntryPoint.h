#pragma once
#include "Graphen/Core/Core.h"

#ifdef HZ_PLATFORM_WINDOWS

extern gn::Application* gn::CreateApplication();

int main(int argc, char** argv)
{
	gn::Log::Init();

	HZ_PROFILE_BEGIN_SESSION("Startup", "GraphenProfile-Startup.json");
	auto app = gn::CreateApplication();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "GraphenProfile-Runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Startup", "GraphenProfile-Shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();
}

#endif
