cmake_minimum_required(VERSION 3.13)
project(maze_solver_cpp)
include_directories(/opt/X11/include)
link_directories(/opt/X11/lib)
add_definitions(-std=gnu++0x)
set(CMAKE_CXX_FLAGS "-lX11")
add_executable(maze_solver_cpp test.cpp BFS.cpp)