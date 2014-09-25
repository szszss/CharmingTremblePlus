project "Game"
configuration "Release"
	kind "WindowedApp"
configuration "Debug"
	kind "ConsoleApp"
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