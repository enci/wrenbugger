#include "debugger_ui.h"
#include "wren_interface.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <sstream>

using namespace ftxui;

debugger_ui::debugger_ui(std::shared_ptr<wren_interface> wren)
    : wren_(wren), screen_(ScreenInteractive::Fullscreen()) {
}

debugger_ui::~debugger_ui() {
}

Component debugger_ui::create_source_view() {
    auto renderer = Renderer([&] {
        // Split script content into lines
        std::string content = wren_->get_script_content();
        std::vector<std::string> lines;
        std::istringstream stream(content);
        std::string line;
        int line_num = 1;
        
        Elements line_elements;
        while (std::getline(stream, line)) {
            line_elements.push_back(
                hbox({
                    text(std::to_string(line_num) + ": ") | dim,
                    text(line)
                })
            );
            line_num++;
        }
        
        if (line_elements.empty()) {
            line_elements.push_back(text("No source loaded") | dim);
        }
        
        return window(text("📄 Source Code") | bold,
            vbox(line_elements) | vscroll_indicator | yframe | flex
        ) | flex;
    });
    return renderer;
}

Component debugger_ui::create_variables_view() {
    auto renderer = Renderer([&] {
        auto vars = wren_->get_variables();
        Elements var_elements;
        for (const auto& var : vars) {
            var_elements.push_back(text("  " + var));
        }
        
        return window(text("🔍 Variables") | bold,
            vbox(var_elements) | yframe
        );
    });
    return renderer;
}

Component debugger_ui::create_callstack_view() {
    auto renderer = Renderer([&] {
        auto stack = wren_->get_callstack();
        Elements stack_elements;
        int frame_num = 0;
        for (const auto& frame : stack) {
            stack_elements.push_back(
                text("  #" + std::to_string(frame_num) + " " + frame)
            );
            frame_num++;
        }
        
        return window(text("📚 Call Stack") | bold,
            vbox(stack_elements) | yframe
        );
    });
    return renderer;
}

Component debugger_ui::create_info_view() {
    auto quit_btn = Button("Quit (Q)", [&] { screen_.ExitLoopClosure()(); });
    
    auto renderer = Renderer(quit_btn, [&] {
        return window(text("ℹ️  Info") | bold,
            vbox({
                text("Wrenbugger - Terminal Debugger for Wren") | bold,
                separator(),
                text(wren_->get_current_line()),
                separator(),
                hbox({
                    text("Press "),
                    text("Q") | bold,
                    text(" to quit")
                }),
                separator(),
                quit_btn->Render() | center
            })
        );
    });
    
    return renderer;
}

Component debugger_ui::create_main_component() {
    auto source = create_source_view();
    auto variables = create_variables_view();
    auto callstack = create_callstack_view();
    auto info = create_info_view();
    
    auto side_panel = Container::Vertical({
        variables,
        callstack,
        info,
    });
    
    auto layout = Container::Horizontal({
        source,
        side_panel,
    });
    
    auto main_renderer = Renderer(layout, [&] {
        return hbox({
            source->Render() | flex,
            separator(),
            vbox({
                variables->Render() | size(HEIGHT, LESS_THAN, 12),
                separator(),
                callstack->Render() | size(HEIGHT, LESS_THAN, 10),
                separator(),
                info->Render() | flex,
            }) | size(WIDTH, EQUAL, 45),
        }) | border;
    });
    
    // Add keyboard shortcuts
    auto with_shortcuts = CatchEvent(main_renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q')) {
            screen_.ExitLoopClosure()();
            return true;
        } else if (event == Event::Escape) {
            screen_.ExitLoopClosure()();
            return true;
        }
        return false;
    });
    
    return with_shortcuts;
}

void debugger_ui::run() {
    auto main_component = create_main_component();
    screen_.Loop(main_component);
}

void debugger_ui::update_display() {
    screen_.PostEvent(Event::Custom);
}
