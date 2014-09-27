project "GLFW"
language "C"
kind "StaticLib"
targetdir "../bin/output"
includedirs {
}
files {
	"src/glfw_config.h",
	"src/internal.h",
	"include/GLFW/glfw3.h",
	"include/GLFW/glfw3native.h",
	"src/clipboard.c",
	"src/context.c",
	"src/gamma.c",
	"src/init.c",
	"src/input.c",
	"src/joystick.c",
	"src/monitor.c",
	"src/time.c",
	"src/window.c",
}
configuration {"windows"}
	files {
		"src/win32_platform.h",
		"src/win32_clipboard.c",
		"src/win32_gamma.c",
		"src/win32_init.c",
		"src/win32_joystick.c",
		"src/win32_monitor.c",
		"src/win32_time.c",
		"src/win32_window.c",
		"src/wgl_platform.h",
		"src/wgl_context.c"
	}
configuration {"linux"}
	files {
		"src/x11_platform.h",
		"src/x11_clipboard.c",
		"src/x11_gamma.c",
		"src/x11_init.c",
		"src/x11_joystick.c",
		"src/x11_monitor.c",
		"src/x11_time.c",
		"src/x11_window.c",
		"src/glx_platform.h",
		"src/glx_context.c"
	}
configuration {"macosx"}
	files {
		"src/cocoa_platform.h",
		"src/cocoa_clipboard.c",
		"src/cocoa_gamma.c",
		"src/cocoa_init.c",
		"src/cocoa_joystick.c",
		"src/cocoa_monitor.c",
		"src/cocoa_time.c",
		"src/cocoa_window.c",
		"src/egl_platform.h",
		"src/egl_context.c"
	}
configuration {}
vpaths {
   ["Headers"] = {"**.h", "**.hpp"},
   ["Sources"] = {"**.c", "**.cpp"}
}