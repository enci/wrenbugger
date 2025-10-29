#pragma once

#include <ftxui/component/component.hpp>
#include <memory>

class wren_interface;

// Terminal UI for the debugger using FTXUI
class debugger_ui {
public:
    debugger_ui(std::shared_ptr<wren_interface> wren);
    ~debugger_ui();

    // Run the UI main loop
    void run();

private:
    ftxui::Component create_main_component();
    ftxui::Component create_source_view();
    ftxui::Component create_variables_view();
    ftxui::Component create_callstack_view();
    ftxui::Component create_controls();
    
    void update_display();
    void handle_continue();
    void handle_step_over();
    void handle_step_into();
    void handle_step_out();

    std::shared_ptr<wren_interface> wren_;
    ftxui::ScreenInteractive screen_;
};
