project "test"
	language "C++"
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
