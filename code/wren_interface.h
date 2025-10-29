#pragma once

extern "C" {
#include "wren.h"
}
#include <string>
#include <vector>

// Forward declare internal type
struct WrenVM;

// Interface to wrap Wren VM and debugger functionality
class wren_interface {
public:
    wren_interface();
    ~wren_interface();

    // Initialize VM with a script file
    bool load_script(const std::string& filename);
    
    // Query current state
    std::string get_current_line() const;
    std::vector<std::string> get_callstack() const;
    std::vector<std::string> get_variables() const;
    std::string get_script_content() const;
    
private:
    WrenVM* vm_;
    WrenConfiguration config_;
    std::string script_source_;
    std::string script_module_;
};
