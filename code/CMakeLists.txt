cmake_minimum_required(VERSION 3.15)

# Debugger executable
add_executable(wrenbugger
    main.cpp
    debugger_ui.cpp
    wren_interface.cpp
)

target_link_libraries(wrenbugger PRIVATE
    wren
    ftxui::screen
    ftxui::dom
    ftxui::component
)

target_include_directories(wrenbugger PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(wrenbugger PRIVATE cxx_std_17)
