#pragma once
#include "types.h"
#include <vector>

namespace orbit {
    std::vector<Vec2> predict(Vec2 pos, Vec2 vel, Body& body, float G, float fixedDt);
    OrbitAnalysis analyze(Ship& ship, Body& body, float G, float fixedDt);
}
