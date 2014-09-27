project "Game"
language "C++"
configuration "Release"
	kind "WindowedApp"
configuration "Debug"
	kind "ConsoleApp"
configuration {}
targetdir "../bin"
links {"GLFW","STB"}
includedirs {
	"../lib/glfw/include/",
	"../lib/stb/include/"
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