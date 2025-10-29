cmake_minimum_required(VERSION 3.15)
project(wrenbugger LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add Wren
add_subdirectory(external/wren)

# Add FTXUI
add_subdirectory(external/ftxui)

# Add our code
add_subdirectory(code)
