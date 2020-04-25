#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "Common.h"

namespace gn {

	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define HZ_CORE_TRACE(...)    ::gn::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HZ_CORE_INFO(...)     ::gn::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_WARN(...)     ::gn::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_ERROR(...)    ::gn::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HZ_CORE_CRITICAL(...) ::gn::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define HZ_TRACE(...)         ::gn::Log::GetClientLogger()->trace(__VA_ARGS__)
#define HZ_INFO(...)          ::gn::Log::GetClientLogger()->info(__VA_ARGS__)
#define HZ_WARN(...)          ::gn::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HZ_ERROR(...)         ::gn::Log::GetClientLogger()->error(__VA_ARGS__)
#define HZ_CRITICAL(...)      ::gn::Log::GetClientLogger()->critical(__VA_ARGS__)