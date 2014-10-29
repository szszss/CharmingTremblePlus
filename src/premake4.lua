project "Game"
language "C++"
configuration "Release"
	kind "WindowedApp"
configuration "Debug"
	kind "ConsoleApp"
configuration {}
local suf
configuration {"x32"}
	targetdir ("../bin/install_x86")
	debugdir ("../bin/install_x86")
configuration "x64"		
	targetdir ("../bin/install_x64")
	debugdir ("../bin/install_x64")
configuration {"x64", "debug"}
	targetdir ("../bin/install_x64_debug")
	debugdir ("../bin/install_x64_debug")
configuration {"x64", "release"}
	targetdir ("../bin/install_x64_release")
	debugdir ("../bin/install_x64_release")
configuration {"x32", "debug"}
	targetdir ("../bin/install_x86_debug")
	debugdir ("../bin/install_x86_debug")
configuration{}
configuration {"x32", "Windows"}
	postbuildcommands { "xcopy /e /y \"$(ProjectDir)..\\..\\assets\" \"$(ProjectDir)..\\..\\bin\\install_x86\"",
						"del /q \"$(ProjectDir)..\\..\\bin\\install_x86\\.git\"" }
configuration {"x64", "Windows"}		
	postbuildcommands { "xcopy /e /y \"$(ProjectDir)..\\..\\assets\" \"$(ProjectDir)..\\..\\bin\\install_x64\"",
						"del /q \"$(ProjectDir)..\\..\\bin\\install_x64\\.git\"" }
configuration {"x64", "debug", "Windows"}
	postbuildcommands { "xcopy /e /y \"$(ProjectDir)..\\..\\assets\" \"$(ProjectDir)..\\..\\bin\\install_x64_debug\"",
						"del /q \"$(ProjectDir)..\\..\\bin\\install_x64_debug\\.git\"" }
configuration {"x64", "release", "Windows"}
	postbuildcommands { "xcopy /e /y \"$(ProjectDir)..\\..\\assets\" \"$(ProjectDir)..\\..\\bin\\install_x64_release\"",
						"del /q \"$(ProjectDir)..\\..\\bin\\install_x64_release\\.git\"" }
configuration {"x32", "debug", "Windows"}
	postbuildcommands { "xcopy /e /y \"$(ProjectDir)..\\..\\assets\" \"$(ProjectDir)..\\..\\bin\\install_x86_debug\"",
						"del /q \"$(ProjectDir)..\\..\\bin\\install_x86_debug\\.git\"" }
configuration{}
targetname "CharmingTremblePlus"
targetsuffix ""
links {"SDL","STB"}
includedirs {
	--"../lib/glfw/include/",
	"../lib/sdl/include/",
	"../lib/stb/include/"
	--"../lib/tbox/src/"
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
configuration {"not Windows"}
	--TODO:postbuildcommands
configuration{}