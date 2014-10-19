project "Game"
language "C++"
configuration "Release"
	kind "WindowedApp"
configuration "Debug"
	kind "ConsoleApp"
configuration {}
targetdir "../bin"
links {"SDL","STB","TBox"}
includedirs {
	"../lib/glfw/include/",
	"../lib/sdl/include/",
	"../lib/stb/include/",
	"../lib/tbox/src/"
}
files {
	"*.c",
	"*.cpp",
	"*.h",
	"*.hpp"
}
vpaths {
   ["Headers"] = {"**.h", "**.hpp"},
   ["Sources"] = {"**.c", "**.cpp"}
}