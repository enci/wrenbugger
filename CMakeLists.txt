cmake_minimum_required(VERSION 3.26)
project(wrenbugger VERSION 0.1
    DESCRIPTION "Terminal debugger for Wren"
    LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/executable)

include(FetchContent)

# Fetch Wren (debugger branch)
FetchContent_Declare(
    wren
    GIT_REPOSITORY https://github.com/wren-lang/wren.git
    GIT_TAG debugger
)

# Fetch FTXUI
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v5.0.0
)

set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(ftxui)
FetchContent_GetProperties(wren)
if(NOT wren_POPULATED)
    FetchContent_Populate(wren)
endif()

# Build Wren as a static library
file(GLOB WREN_SOURCES 
    ${wren_SOURCE_DIR}/src/vm/*.c
    ${wren_SOURCE_DIR}/src/optional/*.c
)
add_library(wren STATIC ${WREN_SOURCES})
target_include_directories(wren PUBLIC 
    ${wren_SOURCE_DIR}/src/include
    ${wren_SOURCE_DIR}/src/vm
    ${wren_SOURCE_DIR}/src/optional
)

# Wrenbugger executable
add_executable(wrenbugger
    code/source/main.cpp
    code/source/debugger.cpp
    code/source/wren_vm.cpp
    code/source/ui.cpp
    code/source/display.cpp)

target_include_directories(wrenbugger
    PRIVATE code/include
    PRIVATE ${wren_SOURCE_DIR}/src/include
    PRIVATE ${wren_SOURCE_DIR}/src/vm)

target_link_libraries(wrenbugger
    PRIVATE wren
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component)
