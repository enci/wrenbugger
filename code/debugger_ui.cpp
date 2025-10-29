#include "debugger_ui.h"
#include "wren_interface.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

debugger_ui::debugger_ui(std::shared_ptr<wren_interface> wren)
    : wren_(wren), screen_(ScreenInteractive::Fullscreen()) {
}

debugger_ui::~debugger_ui() {
}

Component debugger_ui::create_source_view() {
    auto renderer = Renderer([&] {
        return window(text("Source Code") | bold,
            vbox({
                text("// Source code view"),
                text(wren_->get_current_line()),
                text("// Line numbers and code will appear here"),
            }) | border
        );
    });
    return renderer;
}

Component debugger_ui::create_variables_view() {
    auto renderer = Renderer([&] {
        auto vars = wren_->get_variables();
        Elements var_elements;
        for (const auto& var : vars) {
            var_elements.push_back(text(var));
        }
        
        return window(text("Variables") | bold,
            vbox(var_elements) | border
        );
    });
    return renderer;
}

Component debugger_ui::create_callstack_view() {
    auto renderer = Renderer([&] {
        auto stack = wren_->get_callstack();
        Elements stack_elements;
        for (const auto& frame : stack) {
            stack_elements.push_back(text(frame));
        }
        
        return window(text("Call Stack") | bold,
            vbox(stack_elements) | border
        );
    });
    return renderer;
}

Component debugger_ui::create_controls() {
    auto continue_btn = Button("Continue (F5)", [&] { handle_continue(); });
    auto step_over_btn = Button("Step Over (F10)", [&] { handle_step_over(); });
    auto step_into_btn = Button("Step Into (F11)", [&] { handle_step_into(); });
    auto step_out_btn = Button("Step Out (F12)", [&] { handle_step_out(); });
    auto quit_btn = Button("Quit (Q)", [&] { screen_.ExitLoopClosure()(); });
    
    auto buttons = Container::Horizontal({
        continue_btn,
        step_over_btn,
        step_into_btn,
        step_out_btn,
        quit_btn,
    });
    
    auto renderer = Renderer(buttons, [&] {
        return window(text("Controls") | bold,
            hbox({
                continue_btn->Render() | border,
                separator(),
                step_over_btn->Render() | border,
                separator(),
                step_into_btn->Render() | border,
                separator(),
                step_out_btn->Render() | border,
                separator(),
                quit_btn->Render() | border,
            })
        );
    });
    
    return renderer;
}

Component debugger_ui::create_main_component() {
    auto source = create_source_view();
    auto variables = create_variables_view();
    auto callstack = create_callstack_view();
    auto controls = create_controls();
    
    auto layout = Container::Vertical({
        Container::Horizontal({
            source,
            Container::Vertical({
                variables,
                callstack,
            }),
        }),
        controls,
    });
    
    auto main_renderer = Renderer(layout, [&] {
        return vbox({
            hbox({
                source->Render() | flex,
                separator(),
                vbox({
                    variables->Render() | size(HEIGHT, LESS_THAN, 15),
                    separator(),
                    callstack->Render() | flex,
                }) | size(WIDTH, EQUAL, 40),
            }) | flex,
            separator(),
            controls->Render(),
        }) | border;
    });
    
    // Add keyboard shortcuts
    auto with_shortcuts = CatchEvent(main_renderer, [&](Event event) {
        if (event == Event::F5) {
            handle_continue();
            return true;
        } else if (event == Event::F10) {
            handle_step_over();
            return true;
        } else if (event == Event::F11) {
            handle_step_into();
            return true;
        } else if (event == Event::F12) {
            handle_step_out();
            return true;
        } else if (event == Event::Character('q') || event == Event::Character('Q')) {
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

void debugger_ui::handle_continue() {
    wren_->continue_execution();
    update_display();
}

void debugger_ui::handle_step_over() {
    wren_->step_over();
    update_display();
}

void debugger_ui::handle_step_into() {
    wren_->step_into();
    update_display();
}

void debugger_ui::handle_step_out() {
    wren_->step_out();
    update_display();
}
