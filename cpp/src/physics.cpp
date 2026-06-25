#include "physics.h"
#include <cmath>

namespace physics {

// 牛顿万有引力：F = GMm/r²，返回加速度矢量
Vec2 gravity(Vec2 shipPos, Vec2 bodyPos, float bodyMass, float G) {
    float dx = bodyPos.x - shipPos.x;
    float dy = bodyPos.y - shipPos.y;
    float distSq = dx * dx + dy * dy;
    float dist = std::sqrt(distSq);
    if (dist < 1.0f) return {0, 0};
    float force = G * bodyMass / distSq;
    return {force * dx / dist, force * dy / dist};
}

// 将油门转化为推力加速度
void applyThrust(Ship& ship, float thrustMax, float dt) {
    if (ship.throttle > 0.01f) {
        ship.vel.x += thrustMax * ship.throttle * std::cos(ship.angle) * dt;
        ship.vel.y += thrustMax * ship.throttle * std::sin(ship.angle) * dt;
    }
}

// 半隐式欧拉积分：先更新速度，再更新位置（比显式欧拉更稳定）
void integrate(Ship& ship, Vec2 accel, float dt) {
    ship.vel.x += accel.x * dt;
    ship.vel.y += accel.y * dt;
    ship.pos.x += ship.vel.x * dt;
    ship.pos.y += ship.vel.y * dt;
}

}
