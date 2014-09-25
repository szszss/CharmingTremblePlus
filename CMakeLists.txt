cmake_minimum_required(VERSION 2.8)

project(CharmingTremblePlus CXX)

set(CMAKE_BUILD_TYPE Release)

include_directories(
	${CharmingTremblePlus_SOURCE_DIR}/lib/glfw/include
)

if (WIN32)
	set(GAME_OUTPUT_SUFFIX .exe)
else()
	set(GAME_OUTPUT_SUFFIX )
endif()

aux_source_directory(src/ SRC_MAIN)
add_executable(game ${SRC_MAIN})
set_target_properties(game PROPERTIES OUTPUT_NAME "CharmingTremblePlus${GAME_OUTPUT_SUFFIX}")
set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(lib/glfw)
add_subdirectory(lib/stb)
target_link_libraries(game glfw)
target_link_libraries(game stb)