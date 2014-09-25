project "GLFW"
kind "StaticLib"
targetdir "../bin/output"
includedirs {
}
files {
	"src/*.c",
	"src/*.cpp",
	"include/GLFW/*.h",
	"include/GLFW/*.hpp"
}

local p = project()
p.basedir = "../lib/glfw/"