#pragma once
#include "types.h"

class Camera {
public:
    float x = 0, y = 0;
    void update(Vec2 target, float lerp, int screenW, int screenH);
    Vec2 worldToScreen(Vec2 worldPos, int screenW, int screenH) const;
};
