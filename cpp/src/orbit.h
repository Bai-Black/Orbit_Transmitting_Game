#pragma once
#include "types.h"
#include <vector>

// 轨道预测与分析模块
namespace orbit {
    // 从当前状态向前预测轨迹（用于可视化）
    std::vector<Vec2> predict(Vec2 pos, Vec2 vel, Body& body, float G, float fixedDt);
    // 分析当前轨道：计算近远地点、闭合性（用于稳定性判定）
    OrbitAnalysis analyze(Ship& ship, Body& body, float G, float fixedDt);
}
