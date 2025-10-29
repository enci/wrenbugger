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
