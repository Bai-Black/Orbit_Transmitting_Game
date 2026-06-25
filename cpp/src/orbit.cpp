#include "orbit.h"
#include "physics.h"
#include <cmath>

namespace orbit {

// 预测轨迹：从当前位置/速度向前模拟20秒
std::vector<Vec2> predict(Vec2 pos, Vec2 vel, Body& body, float G, float fixedDt) {
    std::vector<Vec2> points;
    Vec2 px = pos, py = vel;
    float pvx = vel.x, pvy = vel.y;
    const int steps = 2400;

    for (int i = 0; i < steps; i++) {
        points.push_back(px);
        Vec2 g = physics::gravity(px, {body.pos.x, body.pos.y}, body.mass, G);
        pvx += g.x * fixedDt;
        pvy += g.y * fixedDt;
        px.x += pvx * fixedDt;
        px.y += pvy * fixedDt;
        float dx = px.x - body.pos.x;
        float dy = px.y - body.pos.y;
        if (std::sqrt(dx * dx + dy * dy) < body.radius) break;
    }
    return points;
}

// 轨道分析：基于开普勒公式计算轨道参数
// 1. 用 vis-viva 公式计算半长轴 a
// 2. 用开普勒第三定律计算轨道周期 T
// 3. 模拟一个周期，找近远地点和闭合性
OrbitAnalysis analyze(Ship& ship, Body& body, float G, float fixedDt) {
    OrbitAnalysis result;
    float dx = ship.pos.x - body.pos.x;
    float dy = ship.pos.y - body.pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    float speed = std::sqrt(ship.vel.x * ship.vel.x + ship.vel.y * ship.vel.y);
    float GM = G * body.mass;
    // vis-viva 公式：1/a = 2/r - v²/(GM)
    float a = 1.0f / (2.0f / dist - speed * speed / GM);
    // 开普勒第三定律：T = 2π√(a³/GM)
    float period = 2.0f * PI * std::sqrt(std::abs(a) * std::abs(a) * std::abs(a) / GM);
    int steps = std::min((int)std::ceil(period / fixedDt), 3000);

    float px = ship.pos.x, py = ship.pos.y;
    float pvx = ship.vel.x, pvy = ship.vel.y;
    float minDist = dist, maxDist = dist;
    Vec2 periPt = {px, py}, apoPt = {px, py};

    for (int i = 0; i < steps; i++) {
        Vec2 g = physics::gravity({px, py}, {body.pos.x, body.pos.y}, body.mass, G);
        pvx += g.x * fixedDt;
        pvy += g.y * fixedDt;
        px += pvx * fixedDt;
        py += pvy * fixedDt;
        float d = std::sqrt((px - body.pos.x) * (px - body.pos.x) + (py - body.pos.y) * (py - body.pos.y));
        if (d < minDist) { minDist = d; periPt = {px, py}; }
        if (d > maxDist) { maxDist = d; apoPt = {px, py}; }
        // 检查闭合：周期后半段若回到起点附近即闭合
        float dd = std::sqrt((px - ship.pos.x) * (px - ship.pos.x) + (py - ship.pos.y) * (py - ship.pos.y));
        if (i > steps * 0.5 && dd < std::abs(a) * 0.15f) {
            result.closed = true;
            result.periAlt = minDist - body.radius;
            result.apoAlt = maxDist - body.radius;
            result.avgAlt = (minDist + maxDist) / 2.0f - body.radius;
            result.periPoint = periPt;
            result.apoPoint = apoPt;
            return result;
        }
    }

    result.closed = false;
    result.periAlt = minDist - body.radius;
    result.apoAlt = maxDist - body.radius;
    result.avgAlt = (minDist + maxDist) / 2.0f - body.radius;
    result.periPoint = periPt;
    result.apoPoint = apoPt;
    return result;
}

}
