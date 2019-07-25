project "test"
	language "C++"
	cppdialect "C++14"
	kind "ConsoleApp"

	links { "pathfind" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Test files"] = "**.json",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.h",

		"testnodes1.json"
	}
