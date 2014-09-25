include_directories(
	${CharmingTremblePlus_SOURCE_DIR}/lib/glfw/include
)

if (WIN32)
	set(GAME_OUTPUT_SUFFIX .exe)
else()
	set(GAME_OUTPUT_SUFFIX )
endif()

aux_source_directory(. SRC_MAIN)
add_executable(game ${SRC_MAIN});
set_target_properties(game PROPERTIES OUTPUT_NAME "CharmingTremblePlus${GAME_OUTPUT_SUFFIX}")