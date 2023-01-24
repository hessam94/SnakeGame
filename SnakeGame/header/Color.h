#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
using namespace std;
struct Color
{
    static constexpr Color red() { return Color(255, 0, 0); }
    static constexpr Color green() { return Color(0, 255, 0); }
    static constexpr Color blue() { return Color(0, 0, 255); }
    static constexpr Color black() { return Color(0, 0, 0); }
    static constexpr Color white() { return Color(255, 255, 255); }

    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a)
    {}

    constexpr Color()
        : r(255), g(255), b(255), a(255)
    {}

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

#endif