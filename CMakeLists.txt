cmake_minimum_required(VERSION 3.15)
project(wrenbugger LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build Wren library
add_library(wren STATIC
    external/wren/src/vm/wren_compiler.c
    external/wren/src/vm/wren_core.c
    external/wren/src/vm/wren_debug.c
    external/wren/src/vm/wren_debugger.c
    external/wren/src/vm/wren_primitive.c
    external/wren/src/vm/wren_utils.c
    external/wren/src/vm/wren_value.c
    external/wren/src/vm/wren_vm.c
    external/wren/src/optional/wren_opt_meta.c
    external/wren/src/optional/wren_opt_random.c
)

target_include_directories(wren PUBLIC
    external/wren/src/include
    external/wren/src/vm
    external/wren/src/optional
)

target_compile_definitions(wren PUBLIC
    WREN_DEBUGGER=1
)

if(UNIX AND NOT APPLE)
    target_link_libraries(wren PUBLIC m)
endif()

# Add FTXUI
add_subdirectory(external/ftxui)

# Add our code
add_subdirectory(code)
