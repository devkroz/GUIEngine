#pragma once

#include <cstdint>
#include <cmath>

namespace GUIEngine {

struct Color {
    float r, g, b, a;

    constexpr Color() : r(0), g(0), b(0), a(1) {}
    constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r / 255.0f), g(g / 255.0f), b(b / 255.0f), a(a / 255.0f) {}

    static constexpr Color Red()    { return Color(1.0f, 0.0f, 0.0f); }
    static constexpr Color Green()  { return Color(0.0f, 1.0f, 0.0f); }
    static constexpr Color Blue()    { return Color(0.0f, 0.0f, 1.0f); }
    static constexpr Color White()  { return Color(1.0f, 1.0f, 1.0f); }
    static constexpr Color Black()  { return Color(0.0f, 0.0f, 0.0f); }
    static constexpr Color Gray()   { return Color(0.5f, 0.5f, 0.5f); }
    static constexpr Color Dark()   { return Color(0.15f, 0.15f, 0.15f); }
    static constexpr Color Light()  { return Color(0.85f, 0.85f, 0.85f); }
    static constexpr Color Transparent() { return Color(0.0f, 0.0f, 0.0f, 0.0f); }

    Color operator*(float s) const { return Color(r * s, g * s, b * s, a); }
    Color operator+(const Color& o) const { return Color(r + o.r, g + o.g, b + o.b, a + o.a); }
    Color blend(const Color& o) const {
        float inv_a = 1.0f - o.a;
        return Color(r * inv_a + o.r * o.a, g * inv_a + o.g * o.a, b * inv_a + o.b * o.a, a);
    }

    uint32_t toRGBA() const {
        return ((uint32_t)(a * 255) << 24) |
               ((uint32_t)(b * 255) << 16) |
               ((uint32_t)(g * 255) << 8)  |
               ((uint32_t)(r * 255));
    }
};

struct Vec2 {
    float x, y;

    constexpr Vec2() : x(0), y(0) {}
    constexpr Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
    Vec2 operator/(float s) const { return Vec2(x / s, y / s); }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }

    float length() const { return std::sqrt(x * x + y * y); }
    Vec2 normalized() const {
        float len = length();
        if (len > 0.0001f) return Vec2(x / len, y / len);
        return Vec2(0, 0);
    }
};

struct Rect {
    float x, y, width, height;

    constexpr Rect() : x(0), y(0), width(0), height(0) {}
    constexpr Rect(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}

    bool contains(const Vec2& p) const {
        return p.x >= x && p.x <= x + width && p.y >= y && p.y <= y + height;
    }

    bool contains(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    bool intersects(const Rect& o) const {
        return !(x + width < o.x || o.x + o.width < x ||
                 y + height < o.y || o.y + o.height < y);
    }

    Vec2 center() const { return Vec2(x + width * 0.5f, y + height * 0.5f); }
};

}
