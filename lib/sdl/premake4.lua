project "SDL"
language "C"
kind "SharedLib"
local suf
configuration {"x32"}
	suf = "_x86"
	targetdir ("../../bin/install" .. suf)
	targetname ("sdl" .. suf)
configuration "x64"		
	suf = "_x64"
	targetdir ("../../bin/install" .. suf)
	targetname ("sdl" .. suf)
configuration {"x64", "debug"}
	suf = "_x64_debug"
	targetdir ("../../bin/install" .. suf)
	targetname ("sdl" .. suf)
configuration {"x64", "release"}
	suf = "_x64_release" 
	targetdir ("../../bin/install" .. suf)
	targetname ("sdl" .. suf)
configuration {"x32", "debug"}
	suf = "_x86_debug"
	targetdir ("../../bin/install" .. suf)
	targetname ("sdl" .. suf)
configuration{}
targetsuffix ""
objdir "../../bin/obj"
includedirs {
	"include/",
	"src/**"
}
defines {
	
}
files {
	"include/*.h",
	"src/*.c",
	"src/atomic/*.c",
	"src/audio/*.c",
	"src/cpuinfo/*.c",
	"src/dynapi/**",
	"src/events/*.c",
	"src/file/*.c",
	"src/libm/*.c",
 	--"src/render/*.c",
	"src/render/**",
	"src/stdlib/*.c",
	"src/thread/*.c",
	"src/timer/*.c",
	"src/video/*.c",
	"src/joystick/*.c",
	"src/haptic/*.c",
	"src/power/*.c",
	
}
configuration {"windows"}
	files {
		"src/core/windows/*.c",
		"src/audio/winmm/*.c",
		"src/audio/directsound/*.c",
		"src/video/windows/*.c",
		"src/video/dummy/*.c",
		"src/thread/windows/SDL_sysmutex.c",
		"src/thread/windows/SDL_syssem.c",
 		"src/thread/windows/SDL_systhread.c",
		"src/thread/windows/SDL_systls.c",
		"src/thread/generic/SDL_syscond.c",
		"src/power/windows/SDL_syspower.c",
		"src/filesystem/windows/*.c",
		"src/timer/windows/*.c",
		"src/loadso/windows/*.c",
		"src/core/windows/*.c",
		"src/audio/winmm/*.c",
		"src/audio/dummy/*.c",
		"src/audio/disk/*.c",
		"src/audio/xaudio2/*.c",
		"src/joystick/windows/*.c",
		"src/haptic/windows/*.c",
		"src/main/windows/*.rc"
	}
	includedirs {"$(DXSDK_DIR)/include"}
	links{"winmm","imm32","oleaut32","version"}
configuration {"windows", "x32"}
	libdirs {"$(DXSDK_DIR)/lib/x86"}
configuration {"windows", "x64"}
	libdirs {"$(DXSDK_DIR)/lib/x64"}
configuration {"windows", "release"}
	buildoptions {"/MD"}
configuration {"windows", "debug"}
	buildoptions {"/MDd"}
configuration {}
--[[configuration {"linux"}
	files {
		"src/core/linux/*.c",
	}
	defines {
		"_GLFW_X11",
		"_GLFW_GLX"
	}
configuration {"macosx"}
	files {
		"src/cocoa_platform.h",
		"src/cocoa_clipboard.m",
		"src/cocoa_gamma.c",
		"src/cocoa_init.m",
		"src/cocoa_joystick.m",
		"src/cocoa_monitor.m",
		"src/cocoa_time.c",
		"src/cocoa_window.m",
		"src/nsgl_platform.h",
		"src/nsgl_context.m"
	}
	defines {
		"_GLFW_COCOA",
		"_GLFW_NSGL"
	}
configuration {}
]]--
vpaths {
   ["Headers"] = {"**.h", "**.hpp"},
   ["Sources"] = {"**.c", "**.cpp", "**.rc"}
}