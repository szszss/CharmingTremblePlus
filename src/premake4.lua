project "Game"
language "C++"
configuration "Release"
	kind "WindowedApp"
configuration "Debug"
	kind "ConsoleApp"
configuration {}
targetdir ("../bin/install" .. suffix)
debugdir ("../bin/install" .. suffix)
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

configuration {"Windows"}
	defines { "_CRT_SECURE_NO_WARNINGS","_CRT_SECURE_NO_DEPRECATE"}
	--prebuildcommands  {"del /s /q \"$(ProjectDir)..\\..\\bin\\install" .. suffix .."\""}
	postbuildcommands { "xcopy /e /y \"$(ProjectDir)..\\..\\assets\" \"$(ProjectDir)..\\..\\bin\\install" .. suffix .."\"",
						"del /q \"$(ProjectDir)..\\..\\bin\\install" .. suffix .."\\.git\""
						--" move /y     \"..\\..\\bin\\Game" .. suffix .."\" \"..\\..\\bin\\install" .. suffix .. "\\CharmingTremblePlus.exe\" ",
						--" move /y     \"..\\..\\bin\\SDL" .. suffix .."\"  \"..\\..\\bin\\install" .. suffix .. "\\\" "
						}
configuration {"not Windows"}
	--TODO:postbuildcommands
configuration{}