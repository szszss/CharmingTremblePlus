solution "CharmingTremblePlus"

configurations {"Release", "Debug"}
configuration "Release"
	flags { "Optimize", "EnableSSE", "StaticRuntime", "NoMinimalRebuild", "FloatFast"}
configuration "Debug"
	flags { "Symbols", "StaticRuntime" , "NoMinimalRebuild", "NoEditAndContinue" ,"FloatFast"}
configuration{}

if os.is("Linux") then
    if os.is64bit() then
        platforms {"x64"}
    else
        platforms {"x32"}
    end
else
    platforms {"x32", "x64"}
end

configuration {"Windows"}
	defines { "_CRT_SECURE_NO_WARNINGS","_CRT_SECURE_NO_DEPRECATE"}
configuration{}
  
if _ACTION == "xcode4" then
	xcodebuildsettings
	{
		'ARCHS = "$(ARCHS_STANDARD_32_BIT) $(ARCHS_STANDARD_64_BIT)"',
		'CLANG_CXX_LANGUAGE_STANDARD = "gnu++0x"',
		'VALID_ARCHS = "x86_64 i386"',
	}
	
end

act = ""
    if _ACTION then
    act = _ACTION
end
configuration {"x32"}
	targetsuffix ("_" .. act)
configuration "x64"		
	targetsuffix ("_" .. act .. "_64" )
configuration {"x64", "debug"}
	targetsuffix ("_" .. act .. "_x64_debug")
configuration {"x64", "release"}
	targetsuffix ("_" .. act .. "_x64_release" )
configuration {"x32", "debug"}
	targetsuffix ("_" .. act .. "_debug" )
configuration{}

targetdir "../bin"
	
language "C++"
	
location("./" .. act)

--dofile("premakeHack.lua")
tbox_float    = true
tbox_platform = true
tbox_asio     = true
tbox_xml      = true
tbox_charset  = true

include "../src/"
--include "../lib/glfw/"
include "../lib/sdl/"
include "../lib/stb/"
include "../lib/tbox/"