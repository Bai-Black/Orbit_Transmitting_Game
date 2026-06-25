#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <string>

static constexpr float PI = 3.14159265358979323846f;

// 二维向量
struct Vec2 {
    float x = 0, y = 0;
    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    float length() const { return x * x + y * y; }
    float lengthSqrt() const { return x * x + y * y; }
};

// 天体（行星/恒星）
struct Body {
    Vec2 pos;       // 位置
    float mass = 0; // 质量
    float radius = 0; // 半径
    std::string label; // 标签
};

// 飞行器
struct Ship {
    Vec2 pos;       // 位置
    Vec2 vel;       // 速度
    float angle = 0; // 朝向角度
    float throttle = 0; // 油门 (0~1)
};

// 轨道分析结果
struct OrbitAnalysis {
    bool closed = false;    // 轨迹是否闭合
    float periAlt = 0;      // 近地点高度
    float apoAlt = 0;       // 远地点高度
    float avgAlt = 0;       // 平均高度
    Vec2 periPoint;         // 近地点位置
    Vec2 apoPoint;          // 远地点位置
};

// 轨道数据（预测轨迹+分析结果）
struct OrbitData {
    std::vector<Vec2> points;   // 预测轨迹点
    OrbitAnalysis analysis;     // 轨道分析
};

// 游戏状态
enum class GameState {
    TITLE,      // 标题画面
    PLAYING,    // 游戏中
    PAUSED,     // 暂停
    WIN,        // 胜利
    CRASH       // 坠毁
};

// 暂停按钮
struct Button {
    int x, y, w, h;
    std::string action;
};
