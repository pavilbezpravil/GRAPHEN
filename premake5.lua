os.copyfile("Graphen/vendor/GLFW_proj/premake5.lua", "Graphen/vendor/GLFW/premake5.lua")
os.copyfile("Graphen/vendor/imgui_proj/premake5.lua", "Graphen/vendor/imgui/premake5.lua")

workspace "Graphen"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["ImGui"] = "Graphen/vendor/imgui"
IncludeDir["stb_image"] = "Graphen/vendor/stb_image"
IncludeDir["winpixevent"] = "Graphen/vendor/winpixeventruntime.1.0.200127001/Include/WinPixEventRuntime"

LinkDir = {}
LinkDir["winpixevent"] = "Graphen/vendor/winpixeventruntime.1.0.200127001/bin/x64"

os.copyfile("Graphen/vendor/winpixeventruntime.1.0.200127001/bin/x64/WinPixEventRuntime.dll", "bin/Debug-windows-x86_64/Sandbox")

group "Dependencies"
	include "Graphen/vendor/imgui"

group ""

project "Graphen"
	location "Graphen"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	-- nuget { "WinPixEventRuntime:1.0.200127001" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "gnpch.h"
	pchsource "Graphen/src/gnpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.winpixevent}",
	}

	libdirs {
		"%{LinkDir.winpixevent}"
	}

	links 
	{ 
		"ImGui",
		"d3d12.lib",
		"dxgi.lib",
		"d3dcompiler.lib",
		"dxguid.lib",
		"WinPixEventRuntime.lib",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Graphen/vendor/spdlog/include",
		"Graphen/src",
		"Graphen/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Graphen",
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"
