cmake_minimum_required(VERSION 3.6)
project(lines)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

set(SOURCE_FILES WIN32 main.cpp lines.cpp lines.h main.h)
add_executable(lines ${SOURCE_FILES})