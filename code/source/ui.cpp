#include "ui.hpp"
#include "wren_vm.hpp"
#include "display.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <fstream>
#include <sstream>

using namespace ftxui;

namespace wrenbugger
{
    debugger_ui::debugger_ui(wren_vm& vm) : vm(vm) 
    {
        // Load source file
        std::ifstream file(vm.get_current_module());
        if(file.is_open())
        {
            std::string line;
            while(std::getline(file, line))
            {
                source_lines.push_back(line);
            }
        }
    }
    
    void debugger_ui::run()
    {
        auto screen = ScreenInteractive::Fullscreen();
        auto main_ui = create_main_layout();
        
        screen.Loop(main_ui);
    }
    
    Component debugger_ui::create_source_view()
    {
        return Renderer([&] {
            Elements lines;
            
            int line_num = 1;
            for(const auto& line : source_lines)
            {
                bool is_current = (line_num == vm.get_current_line());
                bool has_breakpoint = false;
                
                // Check if this line has a breakpoint
                for(const auto& bp : vm.get_breakpoints())
                {
                    if(bp.line == line_num && bp.enabled)
                    {
                        has_breakpoint = true;
                        break;
                    }
                }
                
                auto line_text = text(std::to_string(line_num)) | size(WIDTH, EQUAL, 4);
                auto bp_marker = text(has_breakpoint ? "●" : " ");
                auto code_text = text(line);
                
                auto line_element = hbox({bp_marker, line_text, text(" "), code_text});
                
                if(is_current)
                    line_element = line_element | inverted;
                
                lines.push_back(line_element);
                line_num++;
            }
            
            return vbox({
                text("Source") | bold,
                separator(),
                vbox(lines)
            }) | border | flex;
        });
    }
    
    Component debugger_ui::create_modules_tabs()
    {
        return Renderer([&] {
            auto module_name = vm.get_current_module();
            return hbox({
                text("Module: ") | bold,
                text(module_name)
            }) | border;
        });
    }
    
    Component debugger_ui::create_stack_view()
    {
        return Renderer([&] {
            auto frames = vm.get_stack_trace();
            Elements stack_elements;
            
            for(const auto& frame : frames)
            {
                stack_elements.push_back(
                    text(frame.function + " (" + 
                         frame.module + ":" + 
                         std::to_string(frame.line) + ")")
                );
            }
            
            if(stack_elements.empty())
                stack_elements.push_back(text("No stack trace") | dim);
            
            return vbox({
                text("Call Stack") | bold,
                separator(),
                vbox(stack_elements)
            }) | border | size(HEIGHT, LESS_THAN, 10);
        });
    }
    
    Component debugger_ui::create_variables_view()
    {
        return Renderer([&] {
            auto vars = vm.get_variables();
            Elements var_elements;
            
            for(const auto& var : vars)
            {
                var_elements.push_back(
                    hbox({
                        text(var.name) | bold,
                        text(" = "),
                        text(var.value),
                        text(" (") | dim,
                        text(var.type) | dim,
                        text(")") | dim
                    })
                );
            }
            
            if(var_elements.empty())
                var_elements.push_back(text("No variables") | dim);
            
            return vbox({
                text("Variables") | bold,
                separator(),
                vbox(var_elements)
            }) | border | size(HEIGHT, LESS_THAN, 10);
        });
    }
    
    Component debugger_ui::create_controls()
    {
        auto continue_btn = Button("Continue [F5]", [&] { vm.continue_execution(); });
        auto step_over_btn = Button("Step Over [F10]", [&] { vm.step_over(); });
        auto step_into_btn = Button("Step Into [F11]", [&] { vm.step_into(); });
        auto step_out_btn = Button("Step Out [S-F11]", [&] { vm.step_out(); });
        
        return Container::Horizontal({
            continue_btn,
            step_over_btn,
            step_into_btn,
            step_out_btn
        }) | border;
    }
    
    Component debugger_ui::create_display_view()
    {
        return Renderer([&] {
            auto* disp = vm.get_display();
            const auto& buffer = disp->get_buffer();
            
            Elements rows;
            for(int y = 0; y < 8; ++y)
            {
                Elements cols;
                for(int x = 0; x < 8; ++x)
                {
                    cols.push_back(text(buffer[y][x] ? "█" : "·"));
                }
                rows.push_back(hbox(cols) | center);
            }
            
            return vbox({
                text("Display (8x8)") | bold | center,
                separator(),
                vbox(rows)
            }) | border | size(HEIGHT, LESS_THAN, 12);
        });
    }
    
    Component debugger_ui::create_output_view()
    {
        return Renderer([&] {
            auto output = vm.get_output();
            Elements output_lines;
            
            // Show last 10 lines
            int start = std::max(0, (int)output.size() - 10);
            for(int i = start; i < output.size(); ++i)
            {
                output_lines.push_back(text(output[i]));
            }
            
            if(output_lines.empty())
                output_lines.push_back(text("No output") | dim);
            
            return vbox({
                text("Output") | bold,
                separator(),
                vbox(output_lines)
            }) | border | size(HEIGHT, LESS_THAN, 12);
        });
    }
    
    Component debugger_ui::create_main_layout()
    {
        auto source = create_source_view();
        auto modules = create_modules_tabs();
        auto stack = create_stack_view();
        auto vars = create_variables_view();
        auto controls = create_controls();
        auto display_view = create_display_view();
        auto output = create_output_view();
        
        auto left_panel = Container::Vertical({
            modules,
            source
        });
        
        auto right_panel = Container::Vertical({
            stack,
            vars,
            display_view
        });
        
        auto main_content = Container::Horizontal({
            left_panel | flex,
            right_panel | size(WIDTH, EQUAL, 40)
        });
        
        return Container::Vertical({
            main_content | flex,
            controls,
            output
        });
    }
}
