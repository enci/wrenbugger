#pragma once

#include "wren.h"
#include "wren_debugger.h"
#include <string>
#include <vector>

// Interface to wrap Wren VM and debugger functionality
class wren_interface {
public:
    wren_interface();
    ~wren_interface();

    // Initialize VM with a script file
    bool load_script(const std::string& filename);
    
    // Debugger controls
    void continue_execution();
    void step_over();
    void step_into();
    void step_out();
    void add_breakpoint(const std::string& module, int line);
    void remove_breakpoint(int id);
    
    // Query current state
    bool is_halted() const;
    std::string get_current_line() const;
    std::vector<std::string> get_callstack() const;
    std::vector<std::string> get_variables() const;
    
private:
    WrenVM* vm_;
    WrenConfiguration config_;
    std::string script_source_;
    std::string script_module_;
};
