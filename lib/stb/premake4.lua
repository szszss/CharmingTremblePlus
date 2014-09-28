project "STB"
language "C"
kind "StaticLib"
targetdir "../../bin/obj"
objdir "../../bin/obj"
includedirs {
	"include/"
}
files {
	"src/*.c",
	"src/*.cpp",
	"include/stb/*.h",
	"include/stb/*.hpp"
}
vpaths {
   ["Headers"] = {"**.h", "**.hpp"},
   ["Sources"] = {"**.c", "**.cpp"}
}