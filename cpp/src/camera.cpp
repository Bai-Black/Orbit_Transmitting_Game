#include "camera.h"

void Camera::update(Vec2 target, float lerp, int screenW, int screenH) {
    x += (target.x - x) * lerp;
    y += (target.y - y) * lerp;
}

Vec2 Camera::worldToScreen(Vec2 worldPos, int screenW, int screenH) const {
    return {
        worldPos.x - x + screenW / 2.0f,
        worldPos.y - y + screenH / 2.0f
    };
}
