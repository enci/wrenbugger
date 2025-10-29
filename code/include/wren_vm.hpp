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
