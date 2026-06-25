#include "orbit.h"
#include "physics.h"
#include <cmath>

namespace orbit {

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

OrbitAnalysis analyze(Ship& ship, Body& body, float G, float fixedDt) {
    OrbitAnalysis result;
    float dx = ship.pos.x - body.pos.x;
    float dy = ship.pos.y - body.pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    float speed = std::sqrt(ship.vx * ship.vx + ship.vy * ship.vy);
    float GM = G * body.mass;
    float a = 1.0f / (2.0f / dist - speed * speed / GM);
    float period = 2.0f * (float)M_PI * std::sqrt(std::abs(a) * std::abs(a) * std::abs(a) / GM);
    int steps = std::min((int)std::ceil(period / fixedDt), 3000);

    float px = ship.pos.x, py = ship.pos.y;
    float pvx = ship.vx, pvy = ship.vy;
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
