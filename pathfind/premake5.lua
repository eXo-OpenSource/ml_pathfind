project "pathfind"
	language "C++"
	cppdialect "C++14"
	kind "StaticLib"

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
