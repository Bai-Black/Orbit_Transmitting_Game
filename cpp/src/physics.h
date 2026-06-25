#pragma once
#include "types.h"

// 物理引擎：重力计算、推力应用、数值积分
namespace physics {
    // 计算飞行器受天体的引力加速度
    Vec2 gravity(Vec2 shipPos, Vec2 bodyPos, float bodyMass, float G);
    // 应用推力加速度
    void applyThrust(Ship& ship, float thrustMax, float dt);
    // 半隐式欧拉积分：更新速度和位置
    void integrate(Ship& ship, Vec2 accel, float dt);
}
