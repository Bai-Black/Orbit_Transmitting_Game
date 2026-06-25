#pragma once
#include <vector>
#include <string>

struct Vec2 {
    float x = 0, y = 0;
    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    float length() const { return x * x + y * y; }
    float lengthSqrt() const { return x * x + y * y; }
};

struct Body {
    Vec2 pos;
    float mass = 0;
    float radius = 0;
    std::string label;
};

struct Ship {
    Vec2 pos;
    Vec2 vel;
    float angle = 0;
    float throttle = 0;
};

struct OrbitAnalysis {
    bool closed = false;
    float periAlt = 0;
    float apoAlt = 0;
    float avgAlt = 0;
    Vec2 periPoint;
    Vec2 apoPoint;
};

struct OrbitData {
    std::vector<Vec2> points;
    OrbitAnalysis analysis;
};

enum class GameState {
    TITLE,
    PLAYING,
    PAUSED,
    WIN,
    CRASH
};

struct Button {
    int x, y, w, h;
    std::string action;
};
