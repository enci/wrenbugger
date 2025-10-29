#pragma once
#include <ftxui/component/component.hpp>
#include <memory>
#include <vector>

namespace wrenbugger
{
    class wren_vm;

    class debugger_ui
    {
    public:
        debugger_ui(wren_vm& vm);
        void run();
        
    private:
        ftxui::Component create_source_view();
        ftxui::Component create_modules_tabs();
        ftxui::Component create_stack_view();
        ftxui::Component create_variables_view();
        ftxui::Component create_controls();
        ftxui::Component create_display_view();
        ftxui::Component create_output_view();
        ftxui::Component create_main_layout();
        
        wren_vm& vm;
        int selected_tab = 0;
        int selected_line = 0;
        std::vector<std::string> source_lines;
    };
}
