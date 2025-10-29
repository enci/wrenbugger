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
