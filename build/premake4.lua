solution "CharmingTremblePlus"

configurations {"Release", "Debug"}
configuration "Release"
	flags { "Optimize", "EnableSSE", "Unicode"}
configuration "Debug"
	flags { "Symbols", "NoEditAndContinue" ,"Unicode"}
	defines { "DEBUG"}
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
	suffix = "_" .. act
configuration "x64"		
	suffix = "_" .. act .. "_64"
configuration {"x64", "debug"}
	suffix = "_" .. act .. "_x64_debug"
configuration {"x64", "release"}
	suffix = "_" .. act .. "_x64_release" 
configuration {"x32", "debug"}
	suffix = "_" .. act .. "_debug"
configuration{}

targetsuffix (suffix)

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
--include "../lib/tbox/"