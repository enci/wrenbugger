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
    
    // For now, just validate the script can be interpreted
    // In a full debugger, we'd set up breakpoints and step through
    WrenInterpretResult result = wrenInterpret(vm_, script_module_.c_str(), script_source_.c_str());
    
    return result == WREN_RESULT_SUCCESS;
}

std::string wren_interface::get_current_line() const {
    return "Debugging session started";
}

std::vector<std::string> wren_interface::get_callstack() const {
    std::vector<std::string> stack;
    stack.push_back("main()");
    return stack;
}

std::vector<std::string> wren_interface::get_variables() const {
    std::vector<std::string> vars;
    vars.push_back("Variables will be shown here during debugging");
    return vars;
}

std::string wren_interface::get_script_content() const {
    return script_source_;
}
