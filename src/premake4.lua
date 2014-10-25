project "Game"
language "C++"
configuration "Release"
	kind "WindowedApp"
configuration "Debug"
	kind "ConsoleApp"
configuration {}
local suf
configuration {"x32"}
	suf = "_x86"
configuration "x64"		
	suf = "_x64"
configuration {"x64", "debug"}
	suf = "_x64_debug"
configuration {"x64", "release"}
	suf = "_x64_release" 
configuration {"x32", "debug"}
	suf = "_x86_debug"
configuration{}
targetdir ("../bin/install" .. suf)
debugdir ("../bin/install" .. suf)
targetname "CharmingTremblePlus"
targetsuffix ""
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
	--prebuildcommands  {"del /s /q \"$(ProjectDir)..\\..\\bin\\install" .. suf .."\""}
	postbuildcommands { "xcopy /e /y \"$(ProjectDir)..\\..\\assets\" \"$(ProjectDir)..\\..\\bin\\install" .. suf .."\"",
						"del /q \"$(ProjectDir)..\\..\\bin\\install" .. suf .."\\.git\""
						--" move /y     \"..\\..\\bin\\Game" .. suf .."\" \"..\\..\\bin\\install" .. suf .. "\\CharmingTremblePlus.exe\" ",
						--" move /y     \"..\\..\\bin\\SDL" .. suf .."\"  \"..\\..\\bin\\install" .. suf .. "\\\" "
						}
configuration {"not Windows"}
	--TODO:postbuildcommands
configuration{}