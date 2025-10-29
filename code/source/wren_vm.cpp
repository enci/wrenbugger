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
