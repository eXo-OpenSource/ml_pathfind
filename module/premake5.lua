project "module"
	language "C++"
	cppdialect "C++14"
	kind "SharedLib"
	targetname "ml_pathfind"
	
	includedirs { "include" }
	libdirs { "lib" }

	links { "pathfind" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.h"
	}
	
	filter { "system:windows", "platforms:x86" }
		links { "lua5.1.lib" }
		
	filter { "system:windows", "platforms:x64" }
		links { "lua5.1_64.lib" }

	filter "system:not linux"
		excludes { "luaimports/luaimports.linux.h", "luaimports/luaimports.linux.cpp" }
