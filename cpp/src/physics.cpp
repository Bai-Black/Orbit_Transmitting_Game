#include "physics.h"
#include <cmath>

namespace physics {

Vec2 gravity(Vec2 shipPos, Vec2 bodyPos, float bodyMass, float G) {
    float dx = bodyPos.x - shipPos.x;
    float dy = bodyPos.y - shipPos.y;
    float distSq = dx * dx + dy * dy;
    float dist = std::sqrt(distSq);
    if (dist < 1.0f) return {0, 0};
    float force = G * bodyMass / distSq;
    return {force * dx / dist, force * dy / dist};
}

void applyThrust(Ship& ship, float thrustMax, float dt) {
    if (ship.throttle > 0.01f) {
        ship.vx += thrustMax * ship.throttle * std::cos(ship.angle) * dt;
        ship.vy += thrustMax * ship.throttle * std::sin(ship.angle) * dt;
    }
}

void integrate(Ship& ship, Vec2 accel, float dt) {
    ship.vx += accel.x * dt;
    ship.vy += accel.y * dt;
    ship.pos.x += ship.vx * dt;
    ship.pos.y += ship.vy * dt;
}

}
