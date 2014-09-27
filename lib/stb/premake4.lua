project "STB"
language "C"
kind "StaticLib"
targetdir "../../bin/obj"
objdir "../../bin/obj"
includedirs {
}
files {
	"src/*.c",
	"src/*.cpp",
	"include/STB/*.h",
	"include/STB/*.hpp"
}
vpaths {
   ["Headers"] = {"**.h", "**.hpp"},
   ["Sources"] = {"**.c", "**.cpp"}
}