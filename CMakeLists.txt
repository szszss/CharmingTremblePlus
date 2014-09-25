cmake_minimum_required(VERSION 2.8)

project(CharmingTremblePlus CXX)

add_subdirectory(lib/glfw)
add_subdirectory(lib/stb)
add_subdirectory(src)
target_link_libraries(game glfw)
target_link_libraries(game stb)