#include "debugger_ui.h"
#include "wren_interface.h"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    std::cout << "🐛 Wrenbugger - Terminal Debugger for Wren" << std::endl;
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <script.wren>" << std::endl;
        return 1;
    }
    
    auto wren = std::make_shared<wren_interface>();
    
    std::cout << "Loading script: " << argv[1] << std::endl;
    if (!wren->load_script(argv[1])) {
        std::cerr << "Failed to load script" << std::endl;
        return 1;
    }
    
    std::cout << "Starting debugger UI..." << std::endl;
    debugger_ui ui(wren);
    ui.run();
    
    return 0;
}
