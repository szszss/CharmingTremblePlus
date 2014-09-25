project "STB"
kind "StaticLib"
targetdir "../bin/output"
includedirs {
}
files {
	"src/*.c",
	"src/*.cpp",
	"include/STB/*.h",
	"include/STB/*.hpp"
}