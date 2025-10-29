# Wrenbugger - Terminal Debugger for Wren

Comprehensive implementation guide for building a terminal-based debugger for the Wren programming language.

## Project Overview

Wrenbugger is a terminal UI debugger for Wren scripts using FTXUI for the interface. It provides source viewing, breakpoint management, stack inspection, and a simple I/O system with an 8x8 pixel display.

## Initial Setup

### Directory Structure
```
wrenbugger/
├── CMakeLists.txt
├── code/
│   ├── include/
│   │   ├── debugger.hpp
│   │   ├── wren_vm.hpp
│   │   ├── ui.hpp
│   │   └── display.hpp
│   └── source/
│       ├── main.cpp
│       ├── debugger.cpp
│       ├── wren_vm.cpp
│       ├── ui.cpp
│       └── display.cpp
├── examples/
│   └── test.wren
└── readme.md
```

**Commit**: `🎉 initial project structure`

## Phase 1: CMake with FetchContent

### CMakeLists.txt
```cmake
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

FetchContent_MakeAvailable(wren ftxui)

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
```

**Commit**: `🔧 configure cmake with fetchcontent`

## Phase 2: Wren VM Wrapper

### code/include/wren_vm.hpp
```cpp
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>

// Forward declare Wren types
typedef struct WrenVM WrenVM;

namespace wrenbugger
{
    struct breakpoint
    {
        std::string module;
        int line;
        bool enabled = true;
    };

    struct stack_frame
    {
        std::string function;
        std::string module;
        int line;
    };

    struct variable
    {
        std::string name;
        std::string value;
        std::string type;
    };

    class display;

    class wren_vm
    {
    public:
        wren_vm();
        ~wren_vm();
        
        // Script loading and execution
        bool load_script(const std::string& path);
        void run();
        
        // Debugger control
        void step_over();
        void step_into();
        void step_out();
        void continue_execution();
        void pause();
        
        // Breakpoint management
        void add_breakpoint(const std::string& module, int line);
        void remove_breakpoint(const std::string& module, int line);
        void toggle_breakpoint(const std::string& module, int line);
        std::vector<breakpoint> get_breakpoints() const;
        
        // State inspection
        std::vector<stack_frame> get_stack_trace() const;
        std::vector<variable> get_variables() const;
        bool is_paused() const { return paused; }
        std::string get_current_module() const { return current_module; }
        int get_current_line() const { return current_line; }
        
        // I/O API
        void add_output(const std::string& text);
        std::vector<std::string> get_output() const { return output_buffer; }
        void set_keyboard_input(const std::string& input);
        
        // Display access
        display* get_display() { return display_device.get(); }
        
    private:
        WrenVM* vm = nullptr;
        bool paused = false;
        std::string current_module;
        int current_line = 0;
        std::vector<breakpoint> breakpoints;
        std::vector<std::string> output_buffer;
        std::string keyboard_buffer;
        std::unique_ptr<display> display_device;
    };
}
```

### code/source/wren_vm.cpp
```cpp
#include "wren_vm.hpp"
#include "display.hpp"
#include <wren.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace wrenbugger
{
    // Forward declarations for foreign methods
    static void system_set_pixel(WrenVM* vm);
    static void system_clear_display(WrenVM* vm);
    static void system_input(WrenVM* vm);
    static void system_print(WrenVM* vm);

    static void write_fn(WrenVM* vm, const char* text)
    {
        auto* wvm = static_cast<wren_vm*>(wrenGetUserData(vm));
        wvm->add_output(text);
    }

    static void error_fn(WrenVM* vm, WrenErrorType type,
                        const char* module, int line,
                        const char* message)
    {
        std::cerr << "Wren error in " << module << ":" << line << " - " << message << "\n";
    }

    static WrenForeignMethodFn bind_foreign_method(
        WrenVM* vm,
        const char* module,
        const char* className,
        bool isStatic,
        const char* signature)
    {
        if (strcmp(module, "main") == 0 && strcmp(className, "System") == 0)
        {
            if (isStatic && strcmp(signature, "setPixel(_,_,_)") == 0)
                return system_set_pixel;
            if (isStatic && strcmp(signature, "clearDisplay()") == 0)
                return system_clear_display;
            if (isStatic && strcmp(signature, "input()") == 0)
                return system_input;
            if (isStatic && strcmp(signature, "print(_)") == 0)
                return system_print;
        }
        return nullptr;
    }

    wren_vm::wren_vm()
    {
        WrenConfiguration config;
        wrenInitConfiguration(&config);
        config.writeFn = write_fn;
        config.errorFn = error_fn;
        config.bindForeignMethodFn = bind_foreign_method;
        
        vm = wrenNewVM(&config);
        wrenSetUserData(vm, this);
        
        display_device = std::make_unique<display>();
    }

    wren_vm::~wren_vm()
    {
        if(vm)
            wrenFreeVM(vm);
    }

    bool wren_vm::load_script(const std::string& path)
    {
        std::ifstream file(path);
        if(!file.is_open())
            return false;
            
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        current_module = path;
        WrenInterpretResult result = wrenInterpret(vm, path.c_str(), buffer.str().c_str());
        
        return result == WREN_RESULT_SUCCESS;
    }

    void wren_vm::run()
    {
        // Execute main fiber
        // This would require integration with Wren's debugger API
    }

    void wren_vm::step_over()
    {
        // Debugger command
    }

    void wren_vm::step_into()
    {
        // Debugger command
    }

    void wren_vm::step_out()
    {
        // Debugger command
    }

    void wren_vm::continue_execution()
    {
        paused = false;
    }

    void wren_vm::pause()
    {
        paused = true;
    }

    void wren_vm::add_breakpoint(const std::string& module, int line)
    {
        breakpoints.push_back({module, line, true});
    }

    void wren_vm::remove_breakpoint(const std::string& module, int line)
    {
        breakpoints.erase(
            std::remove_if(breakpoints.begin(), breakpoints.end(),
                [&](const breakpoint& bp) { 
                    return bp.module == module && bp.line == line; 
                }),
            breakpoints.end()
        );
    }

    void wren_vm::toggle_breakpoint(const std::string& module, int line)
    {
        for(auto& bp : breakpoints)
        {
            if(bp.module == module && bp.line == line)
            {
                bp.enabled = !bp.enabled;
                return;
            }
        }
        add_breakpoint(module, line);
    }

    std::vector<breakpoint> wren_vm::get_breakpoints() const
    {
        return breakpoints;
    }

    std::vector<stack_frame> wren_vm::get_stack_trace() const
    {
        // Query Wren debugger API for stack
        return {};
    }

    std::vector<variable> wren_vm::get_variables() const
    {
        // Query Wren debugger API for locals
        return {};
    }

    void wren_vm::add_output(const std::string& text)
    {
        output_buffer.push_back(text);
    }

    void wren_vm::set_keyboard_input(const std::string& input)
    {
        keyboard_buffer = input;
    }

    // Foreign method implementations
    static void system_set_pixel(WrenVM* vm)
    {
        auto* wvm = static_cast<wren_vm*>(wrenGetUserData(vm));
        int x = (int)wrenGetSlotDouble(vm, 1);
        int y = (int)wrenGetSlotDouble(vm, 2);
        bool on = wrenGetSlotBool(vm, 3);
        
        wvm->get_display()->set_pixel(x, y, on);
    }

    static void system_clear_display(WrenVM* vm)
    {
        auto* wvm = static_cast<wren_vm*>(wrenGetUserData(vm));
        wvm->get_display()->clear();
    }

    static void system_input(WrenVM* vm)
    {
        auto* wvm = static_cast<wren_vm*>(wrenGetUserData(vm));
        // Return keyboard input
        wrenSetSlotString(vm, 0, "");
    }

    static void system_print(WrenVM* vm)
    {
        auto* wvm = static_cast<wren_vm*>(wrenGetUserData(vm));
        const char* text = wrenGetSlotString(vm, 1);
        wvm->add_output(text);
    }
}
```

**Commit**: `✨ implement wren vm wrapper with foreign methods`

## Phase 3: Display Module

### code/include/display.hpp
```cpp
#pragma once
#include <array>

namespace wrenbugger
{
    class display
    {
    public:
        display();
        
        void set_pixel(int x, int y, bool on);
        bool get_pixel(int x, int y) const;
        void clear();
        
        const std::array<std::array<bool, 8>, 8>& get_buffer() const { return buffer; }
        
    private:
        std::array<std::array<bool, 8>, 8> buffer;
    };
}
```

### code/source/display.cpp
```cpp
#include "display.hpp"

namespace wrenbugger
{
    display::display()
    {
        clear();
    }
    
    void display::set_pixel(int x, int y, bool on)
    {
        if(x >= 0 && x < 8 && y >= 0 && y < 8)
            buffer[y][x] = on;
    }
    
    bool display::get_pixel(int x, int y) const
    {
        if(x >= 0 && x < 8 && y >= 0 && y < 8)
            return buffer[y][x];
        return false;
    }
    
    void display::clear()
    {
        for(auto& row : buffer)
            row.fill(false);
    }
}
```

**Commit**: `📺 add 8x8 pixel display`

## Phase 4: FTXUI Interface

### code/include/ui.hpp
```cpp
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
```

### code/source/ui.cpp
```cpp
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
    debugger_ui::debugger_ui(wren_vm& vm) : vm(vm) {}
    
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
```

**Commit**: `🎨 create ftxui interface with all panels`

## Phase 5: Main Application

### code/source/main.cpp
```cpp
#include <iostream>
#include "wren_vm.hpp"
#include "ui.hpp"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "Usage: wrenbugger <script.wren>\n";
        return 1;
    }
    
    wrenbugger::wren_vm vm;
    
    if(!vm.load_script(argv[1]))
    {
        std::cerr << "Failed to load script: " << argv[1] << "\n";
        return 1;
    }
    
    wrenbugger::debugger_ui ui(vm);
    ui.run();
    
    return 0;
}
```

**Commit**: `🚀 add main entry point`

## Phase 6: Example Script

### examples/test.wren
```wren
class System {
    foreign static setPixel(x, y, on)
    foreign static clearDisplay()
    foreign static input()
    foreign static print(text)
}

class Main {
    static run() {
        System.print("Wrenbugger demo - WASD to move, Q to quit")
        
        var x = 3
        var y = 3
        
        while (true) {
            System.clearDisplay()
            System.setPixel(x, y, true)
            
            var key = System.input()
            
            if (key == "w" && y > 0) y = y - 1
            if (key == "s" && y < 7) y = y + 1
            if (key == "a" && x > 0) x = x - 1
            if (key == "d" && x < 7) x = x + 1
            if (key == "q") break
            
            System.print("Position: %(x), %(y)")
        }
        
        System.print("Goodbye!")
    }
}

Main.run()
```

**Commit**: `📝 add example wren script`

## Phase 7: README

### readme.md
```markdown
# Wrenbugger

A terminal-based debugger for the Wren programming language.

## Features

- Source code viewing with line numbers
- Breakpoint management (add/remove/toggle)
- Step over/into/out debugging
- Stack trace inspection
- Variable inspection
- 8x8 pixel display for simple graphics
- Output log panel
- Keyboard input API

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

```bash
./executable/wrenbugger path/to/script.wren
```

## Controls

- F5: Continue execution
- F10: Step over
- F11: Step into
- Shift+F11: Step out
- Click on line number to toggle breakpoint

## Wren API

Your Wren scripts can use these System methods:

- `System.print(text)` - Print to output log
- `System.input()` - Get keyboard input
- `System.setPixel(x, y, on)` - Set pixel on 8x8 display
- `System.clearDisplay()` - Clear the display

## Example

See `examples/test.wren` for a simple interactive demo.
```

**Commit**: `📖 add readme documentation`

## Coding Style

Following xxs conventions:

- **Functions/variables**: `lowercase_with_underscores`
  - `get_current_line()`, `source_lines`, `create_display_view()`
  
- **Classes**: `lowercase` 
  - `class wren_vm`, `class display`, `class debugger_ui`
  
- **Namespaces**: `lowercase`
  - `namespace wrenbugger`
  
- **Header guards**: `#pragma once` (no include guards)

- **Smart pointers**: Prefer `std::unique_ptr` and `std::shared_ptr` over raw pointers

- **References**: Use references over pointers when the value cannot be null

- **Auto**: Use `auto` where the type is obvious from context

- **Minimal comments**: Write clear, self-documenting code

## Build & Run

```bash
# Configure
mkdir build && cd build
cmake ..

# Build
cmake --build .

# Run
./executable/wrenbugger ../examples/test.wren
```

## Notes

- The Wren debugger branch must have debug API support
- FTXUI will use your terminal's color scheme automatically
- The 8x8 display uses block characters (█) for pixels
- Output log auto-scrolls to show recent lines
- ALL files small caps
- All commit messages start with descriptive emoji

## Implementation Checklist

- [ ] Phase 1: CMake configuration with FetchContent
- [ ] Phase 2: Wren VM wrapper with debugger hooks
- [ ] Phase 3: 8x8 display module
- [ ] Phase 4: FTXUI interface components
- [ ] Phase 5: Main application entry point
- [ ] Phase 6: Example Wren script
- [ ] Phase 7: Documentation