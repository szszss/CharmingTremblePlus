include_directories(
	${CharmingTremblePlus_SOURCE_DIR}/lib/stb/include
)

aux_source_directory(. SRC_STB)
add_library(stb ${SRC_STB})
#set_target_properties(stb PROPERTIES OUTPUT_NAME "stb")