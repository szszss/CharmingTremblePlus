cmake_minimum_required(VERSION 2.8)

project(CharmingTremblePlus CXX)

set(CMAKE_C_FLAGS "")
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_C_FLAGS_MINSIZEREL "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "")

include_directories(
	${CharmingTremblePlus_SOURCE_DIR}/lib/glfw/include
	${CharmingTremblePlus_SOURCE_DIR}/lib/stb/include
	${CharmingTremblePlus_SOURCE_DIR}/src
)

if (WIN32)
	set(GAME_OUTPUT_SUFFIX .exe)
else()
	set(GAME_OUTPUT_SUFFIX )
endif()

file(GLOB_RECURSE HEADER_MAIN "${CharmingTremblePlus_SOURCE_DIR}/src/*.h")
aux_source_directory(src/ SRC_MAIN)
add_executable(game ${HEADER_MAIN} ${SRC_MAIN})
set_target_properties(game PROPERTIES OUTPUT_NAME "CharmingTremblePlus${GAME_OUTPUT_SUFFIX}")
set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(lib/glfw)
add_subdirectory(lib/stb)
target_link_libraries(game glfw)
target_link_libraries(game stb)
add_dependencies(game glfw stb)