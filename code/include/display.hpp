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
