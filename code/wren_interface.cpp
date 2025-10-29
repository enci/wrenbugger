#include "wren_interface.h"
#include <fstream>
#include <sstream>
#include <iostream>

static void write_fn(WrenVM* vm, const char* text) {
    std::cout << text;
}

static void error_fn(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message) {
    switch (type) {
        case WREN_ERROR_COMPILE:
            std::cerr << "[" << module << " line " << line << "] " << message << std::endl;
            break;
        case WREN_ERROR_RUNTIME:
            std::cerr << "[Runtime Error] " << message << std::endl;
            break;
        case WREN_ERROR_STACK_TRACE:
            std::cerr << "[" << module << " line " << line << "] in " << message << std::endl;
            break;
    }
}

static WrenLoadModuleResult load_module_fn(WrenVM* vm, const char* name) {
    WrenLoadModuleResult result = {};
    result.source = nullptr;
    return result;
}

wren_interface::wren_interface() : vm_(nullptr) {
    wrenInitConfiguration(&config_);
    config_.writeFn = write_fn;
    config_.errorFn = error_fn;
    config_.loadModuleFn = load_module_fn;
    
    vm_ = wrenNewVM(&config_);
}

wren_interface::~wren_interface() {
    if (vm_) {
        wrenFreeVM(vm_);
    }
}

bool wren_interface::load_script(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    script_source_ = buffer.str();
    script_module_ = filename;
    
    // Interpret the script with debugger enabled
    WrenInterpretResult result = wrenInterpret(vm_, script_module_.c_str(), script_source_.c_str());
    
    return result == WREN_RESULT_SUCCESS;
}

void wren_interface::continue_execution() {
    if (vm_) {
        wrenDebuggerSendCommand(vm_, WREN_DEBUGGER_CMD_CONTINUE);
    }
}

void wren_interface::step_over() {
    if (vm_) {
        wrenDebuggerSendCommand(vm_, WREN_DEBUGGER_CMD_STEP_OVER);
    }
}

void wren_interface::step_into() {
    if (vm_) {
        wrenDebuggerSendCommand(vm_, WREN_DEBUGGER_CMD_STEP_INTO);
    }
}

void wren_interface::step_out() {
    if (vm_) {
        wrenDebuggerSendCommand(vm_, WREN_DEBUGGER_CMD_STEP_OUT);
    }
}

void wren_interface::add_breakpoint(const std::string& module, int line) {
    if (vm_) {
        wrenDebuggerAddBreakpoint(vm_, module.c_str(), line);
    }
}

void wren_interface::remove_breakpoint(int id) {
    if (vm_) {
        wrenDebuggerRemoveBreakpoint(vm_, id);
    }
}

bool wren_interface::is_halted() const {
    if (vm_) {
        return wrenDebuggerGetState(vm_) == WREN_DEBUGGER_STATE_HALTING;
    }
    return false;
}

std::string wren_interface::get_current_line() const {
    // TODO: implement getting current line info from debugger
    return "Line info not yet implemented";
}

std::vector<std::string> wren_interface::get_callstack() const {
    std::vector<std::string> stack;
    // TODO: implement getting callstack from debugger
    stack.push_back("Callstack not yet implemented");
    return stack;
}

std::vector<std::string> wren_interface::get_variables() const {
    std::vector<std::string> vars;
    // TODO: implement getting variables from debugger
    vars.push_back("Variables not yet implemented");
    return vars;
}
