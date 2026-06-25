#pragma once
#include "types.h"

namespace physics {
    Vec2 gravity(Vec2 shipPos, Vec2 bodyPos, float bodyMass, float G);
    void applyThrust(Ship& ship, float thrustMax, float dt);
    void integrate(Ship& ship, Vec2 accel, float dt);
}
