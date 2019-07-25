solution "PathFind"
	configurations { "Debug", "Release" }
	location ( "Build" )
	startproject "test"
	targetdir "Bin/%{cfg.buildcfg}"

	platforms { "x86", "x64" }
	pic "On"
	symbols "On"
	
	includedirs { "vendor", "." }

	filter "system:windows"
		defines { "WINDOWS", "WIN32" }

	filter "configurations:Debug"
		defines { "DEBUG" }

	filter "configurations:Release"
		flags { "Optimize" }

	include "module"
	include "pathfind"
	include "test"
