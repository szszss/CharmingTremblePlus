file(GLOB_RECURSE HEADER_STB "${CharmingTremblePlus_SOURCE_DIR}/lib/stb/include/*.h")
aux_source_directory(. SRC_STB)
add_library(stb ${HEADER_STB} ${SRC_STB})
set_target_properties(stb PROPERTIES LINKER_LANGUAGE C)