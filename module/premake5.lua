project "module"
	language "C++"
	kind "SharedLib"
	targetname "ml_gps"
	
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
	
	filter {"system:linux", "platforms:x86" }
			linkoptions { "-Wl,-rpath=mods/deathmatch" }

	filter {"system:linux", "platforms:x64" }
		linkoptions { "-Wl,-rpath=x64" }

	filter "system:linux"
		linkoptions { "-l:lua5.1.so" }

	filter "system:windows"
		links { "lua5.1" }
