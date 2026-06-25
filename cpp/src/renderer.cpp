#include "renderer.h"
#include <cmath>
#include <algorithm>

Renderer::Renderer(SDL_Window* window, SDL_Renderer* renderer)
    : window(window), renderer(renderer) {}

Renderer::~Renderer() {
    if (fontSmall) TTF_CloseFont(fontSmall);
    if (fontMedium) TTF_CloseFont(fontMedium);
    if (fontLarge) TTF_CloseFont(fontLarge);
    if (fontTitle) TTF_CloseFont(fontTitle);
    if (starTexture) SDL_DestroyTexture(starTexture);
}

void Renderer::init() {
    fontSmall = TTF_OpenFont("assets/font.ttf", 10);
    fontMedium = TTF_OpenFont("assets/font.ttf", 12);
    fontLarge = TTF_OpenFont("assets/font.ttf", 16);
    fontTitle = TTF_OpenFont("assets/font.ttf", 48);
    generateStars();
}

void Renderer::generateStars() {
    stars.clear();
    for (int i = 0; i < 300; i++) {
        float x = (float)(rand() % 3000) - 500;
        float y = (float)(rand() % 2000) - 200;
        float brightness = 0.3f + (float)(rand() % 100) / 200.0f;
        stars.push_back({x, y, brightness});
    }

    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, 3000, 2000);
    SDL_SetRenderTarget(renderer, tex);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for (const auto& s : stars) {
        Uint8 c = (Uint8)(s.brightness * 255);
        SDL_SetRenderDrawColor(renderer, c, c, c, 255);
        SDL_Rect r = {(int)(s.x + 500), (int)(s.y + 200), 1, 1};
        SDL_RenderFillRect(renderer, &r);
    }
    SDL_SetRenderTarget(renderer, nullptr);
    starTexture = tex;
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

void Renderer::drawBackground() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Renderer::drawStars(const Camera& cam, int screenW, int screenH) {
    if (!starTexture) return;
    SDL_Rect src = {0, 0, 3000, 2000};
    SDL_Rect dst = {(int)(-500 - cam.x + screenW / 2), (int)(-200 - cam.y + screenH / 2), 3000, 2000};
    SDL_RenderCopy(renderer, starTexture, &src, &dst);
}

void Renderer::drawBody(const Body& body, const Camera& cam, int screenW, int screenH) {
    Vec2 sp = cam.worldToScreen(body.pos, screenW, screenH);
    int sx = (int)sp.x, sy = (int)sp.y;
    int r = (int)body.radius;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    drawCircle(sx, sy, r, {255, 255, 255, 255}, false);

    SDL_SetRenderDrawColor(renderer, 85, 85, 85, 255);
    drawLine(sx - r / 2, sy, sx + r / 2, sy, {85, 85, 85, 255});
    drawLine(sx, sy - r / 2, sx, sy + r / 2, {85, 85, 85, 255});

    drawText(body.label.c_str(), sx, sy + r + 14, {136, 136, 136, 255}, fontSmall);
}

void Renderer::drawTrail(const std::vector<Vec2>& trail, const Camera& cam, int screenW, int screenH) {
    if (trail.size() < 2) return;

    SDL_SetRenderDrawColor(renderer, 51, 51, 51, 255);
    for (size_t i = 1; i < trail.size(); i++) {
        Vec2 p1 = cam.worldToScreen(trail[i - 1], screenW, screenH);
        Vec2 p2 = cam.worldToScreen(trail[i], screenW, screenH);
        drawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, {51, 51, 51, 255});
    }

    for (size_t i = 0; i < trail.size(); i += 6) {
        float alpha = (float)i / trail.size() * 0.5f;
        Vec2 sp = cam.worldToScreen(trail[i], screenW, screenH);
        Uint8 a = (Uint8)(alpha * 255);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, a);
        SDL_Rect r = {(int)sp.x, (int)sp.y, 1, 1};
        SDL_RenderFillRect(renderer, &r);
    }
}

void Renderer::drawShip(const Ship& ship, const Camera& cam, int screenW, int screenH) {
    Vec2 sp = cam.worldToScreen(ship.pos, screenW, screenH);
    int sx = (int)sp.x, sy = (int)sp.y;

    SDL_Point pts[5];
    float c = std::cos(ship.angle), s = std::sin(ship.angle);
    auto rot = [&](float lx, float ly) -> SDL_Point {
        return {(int)(sx + lx * c - ly * s), (int)(sy + lx * s + ly * c)};
    };
    pts[0] = rot(10, 0);
    pts[1] = rot(-6, -5);
    pts[2] = rot(-4, 0);
    pts[3] = rot(-6, 5);
    pts[4] = rot(10, 0);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLines(renderer, pts, 5);

    if (ship.throttle > 0.05f) {
        float flameLen = 4 + ship.throttle * 12;
        for (int i = 0; i < 3; i++) {
            float offset = (float)(i - 1) * 2;
            float len = flameLen * (0.6f + (float)(rand() % 100) / 100.0f * 0.4f);
            Vec2 p1 = cam.worldToScreen({ship.pos.x + (-5 * c - offset * s),
                                          ship.pos.y + (-5 * s + offset * c)}, screenW, screenH);
            Vec2 p2 = cam.worldToScreen({ship.pos.x + ((-5 - len) * c - (offset + (float)(rand() % 100 - 50) / 50.0f) * s),
                                          ship.pos.y + ((-5 - len) * s + (offset + (float)(rand() % 100 - 50) / 50.0f) * c)}, screenW, screenH);
            drawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, {255, 255, 255, 255});
        }
    }
}

void Renderer::drawHUD(const Ship& ship, const Body* bodies, float fuelUsed, GameState state,
                        bool orbitStable, bool orbitReady, float orbitAngleTotal, int screenW, int screenH) {
    float speed = std::sqrt(ship.vx * ship.vx + ship.vy * ship.vy);
    float dist1 = std::sqrt((ship.pos.x - bodies[0].pos.x) * (ship.pos.x - bodies[0].pos.x) +
                            (ship.pos.y - bodies[0].pos.y) * (ship.pos.y - bodies[0].pos.y)) - bodies[0].radius;
    float dist2 = std::sqrt((ship.pos.x - bodies[1].pos.x) * (ship.pos.x - bodies[1].pos.x) +
                            (ship.pos.y - bodies[1].pos.y) * (ship.pos.y - bodies[1].pos.y)) - bodies[1].radius;

    char buf[128];
    snprintf(buf, sizeof(buf), "速度: %.1f", speed);
    drawText(buf, 12, 24, {255, 255, 255, 255}, fontMedium, false);
    snprintf(buf, sizeof(buf), "距 A: %.0f", dist1);
    drawText(buf, 12, 40, {255, 255, 255, 255}, fontMedium, false);
    snprintf(buf, sizeof(buf), "距 B: %.0f", dist2);
    drawText(buf, 12, 56, {255, 255, 255, 255}, fontMedium, false);

    int filled = (int)(ship.throttle * 30);
    snprintf(buf, sizeof(buf), "油门: [%.*s%*s] %.0f%%", filled, "==============================", 30 - filled, "", ship.throttle * 100);
    drawText(buf, 12, 72, {255, 255, 255, 255}, fontMedium, false);
    snprintf(buf, sizeof(buf), "燃料: %.1f", fuelUsed);
    drawText(buf, 12, 88, {255, 255, 255, 255}, fontMedium, false);

    if (state == GameState::PLAYING) {
        if (ship.throttle >= 0.01f) {
            drawText("变轨中", screenW / 2, screenH - 12, {255, 255, 255, 255}, fontMedium);
        } else {
            drawText("无轨道", screenW / 2, screenH - 12, {255, 255, 255, 255}, fontMedium);
        }
    }

    drawText("← → / A D  旋转姿态", screenW - 10, screenH - 44, {102, 102, 102, 255}, fontSmall, false);
    drawText("↑ ↓ / W S  控制油门", screenW - 10, screenH - 32, {102, 102, 102, 255}, fontSmall, false);
    drawText("空格/ESC 暂停  F1 Debug", screenW - 10, screenH - 20, {102, 102, 102, 255}, fontSmall, false);
}

void Renderer::drawTitle(int screenW, int screenH) {
    drawBackground();
    drawStars({0, 0}, screenW, screenH);

    drawText("轨 道 转 移", screenW / 2, screenH / 2 - 80, {255, 255, 255, 255}, fontTitle);
    drawText("ORBITAL TRANSFER", screenW / 2, screenH / 2 - 45, {255, 255, 255, 255}, fontLarge);

    drawCircle(screenW / 2 - 100, screenH / 2 + 20, 12, {255, 255, 255, 255}, false);
    drawCircle(screenW / 2 + 100, screenH / 2 + 20, 8, {255, 255, 255, 255}, false);
    drawDashedCircle(screenW / 2 - 100, screenH / 2 + 20, 50, {51, 51, 51, 255}, 3, 3);

    drawText("← → / A D  旋转姿态", screenW / 2, screenH / 2 + 100, {255, 255, 255, 255}, fontMedium);
    drawText("↑ ↓ / W S  控制油门", screenW / 2, screenH / 2 + 120, {255, 255, 255, 255}, fontMedium);
    drawText("将飞行器变轨至另一天体", screenW / 2, screenH / 2 + 150, {255, 255, 255, 255}, fontMedium);

    static Uint32 lastBlink = 0;
    Uint32 now = SDL_GetTicks();
    if (now - lastBlink > 500) lastBlink = now;
    if ((now / 500) % 2 == 0)
        drawText("[ 按空格键开始 ]", screenW / 2, screenH / 2 + 190, {136, 136, 136, 255}, fontLarge);
}

void Renderer::drawOverlay(const char* title, const char* subtitle, int score, float fuelUsed,
                            const OrbitAnalysis* analysis, int screenW, int screenH) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect full = {0, 0, screenW, screenH};
    SDL_RenderFillRect(renderer, &full);

    drawText(title, screenW / 2, screenH / 2 - 40, {255, 255, 255, 255}, fontTitle);
    drawText(subtitle, screenW / 2, screenH / 2 - 10, {136, 136, 136, 255}, fontLarge);

    if (score > 0) {
        char buf[128];
        float fuelScore = std::max(0.0f, 500.0f - fuelUsed * 50);
        snprintf(buf, sizeof(buf), "燃料效率: %.0f", fuelScore);
        drawText(buf, screenW / 2, screenH / 2 + 20, {170, 170, 170, 255}, fontLarge);
        if (analysis && analysis->apoAlt + analysis->periAlt > 0) {
            float circ = 1.0f - (analysis->apoAlt - analysis->periAlt) / (analysis->apoAlt + analysis->periAlt);
            snprintf(buf, sizeof(buf), "轨道圆度: %.0f", circ * 500);
            drawText(buf, screenW / 2, screenH / 2 + 45, {170, 170, 170, 255}, fontLarge);
        }
        snprintf(buf, sizeof(buf), "总分: %d", score);
        drawText(buf, screenW / 2, screenH / 2 + 80, {255, 255, 255, 255}, fontLarge);
    }

    static Uint32 lastBlink = 0;
    Uint32 now = SDL_GetTicks();
    if (now - lastBlink > 500) lastBlink = now;
    if ((now / 500) % 2 == 0)
        drawText("[ 按空格键重新开始 ]", screenW / 2, screenH / 2 + 120, {255, 255, 255, 255}, fontLarge);
}

void Renderer::drawPause(int screenW, int screenH) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 190);
    SDL_Rect full = {0, 0, screenW, screenH};
    SDL_RenderFillRect(renderer, &full);

    drawText("暂 停", screenW / 2, screenH / 2 - 60, {255, 255, 255, 255}, fontTitle);

    int bw = 180, bh = 40;
    int cx = screenW / 2;

    int continueY = screenH / 2 - 10;
    SDL_Rect r1 = {cx - bw / 2, continueY, bw, bh};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &r1);
    drawText("继 续", cx, continueY + 27, {255, 255, 255, 255}, fontLarge);

    int restartY = screenH / 2 + 50;
    SDL_Rect r2 = {cx - bw / 2, restartY, bw, bh};
    SDL_RenderDrawRect(renderer, &r2);
    drawText("重新开始", cx, restartY + 27, {255, 255, 255, 255}, fontLarge);
}

void Renderer::drawOffscreenIndicator(const Body& target, const Ship& ship, const Camera& cam,
                                       int screenW, int screenH) {
    Vec2 sp = cam.worldToScreen(target.pos, screenW, screenH);
    if (sp.x >= 0 && sp.x <= screenW && sp.y >= 0 && sp.y <= screenH) return;

    float angle = std::atan2(target.pos.y - cam.y, target.pos.x - cam.x);
    float edgeX = std::max(40.0f, std::min((float)screenW - 40, (float)screenW / 2 + std::cos(angle) * (screenW / 2 - 40)));
    float edgeY = std::max(40.0f, std::min((float)screenH - 40, (float)screenH / 2 + std::sin(angle) * (screenH / 2 - 40)));

    float dist = std::sqrt((target.pos.x - ship.pos.x) * (target.pos.x - ship.pos.x) +
                           (target.pos.y - ship.pos.y) * (target.pos.y - ship.pos.y));

    drawText("天体 B", (int)edgeX, (int)edgeY - 12, {136, 136, 136, 255}, fontSmall);
    char buf[32];
    snprintf(buf, sizeof(buf), "%.0f", dist);
    drawText(buf, (int)edgeX, (int)edgeY + 18, {136, 136, 136, 255}, fontSmall);
}

void Renderer::drawOrbitPrediction(const OrbitData* data, const Body& body, bool orbitStable,
                                    const Camera& cam, int screenW, int screenH) {
    Vec2 sp = cam.worldToScreen(body.pos, screenW, screenH);
    int sx = (int)sp.x, sy = (int)sp.y;

    drawDashedCircle(sx, sy, (int)(60 + body.radius), {51, 51, 51, 255}, 4, 6);
    drawDashedCircle(sx, sy, (int)(300 + body.radius), {51, 51, 51, 255}, 4, 6);
    drawText("目标轨道", sx + (int)(300 + body.radius) + 4, sy, {68, 68, 68, 255}, fontLarge, false);

    if (!data) return;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (size_t i = 1; i < data->points.size(); i++) {
        Vec2 p1 = cam.worldToScreen(data->points[i - 1], screenW, screenH);
        Vec2 p2 = cam.worldToScreen(data->points[i], screenW, screenH);
        drawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, {255, 255, 255, 255});
    }

    if (!orbitStable) return;
    const auto& a = data->analysis;

    if (a.periAlt + a.apoAlt > 0 && std::abs(a.apoAlt - a.periAlt) < (a.apoAlt + a.periAlt) * 0.1f) {
        int r = (int)(a.avgAlt + body.radius);
        drawCircle(sx, sy, r, {102, 102, 102, 255}, false);
        char buf[32];
        snprintf(buf, sizeof(buf), "h=%.0f", a.avgAlt);
        drawText(buf, sx + r + 6, sy, {170, 170, 170, 255}, fontSmall, false);
    } else {
        int periR = (int)(a.periAlt + body.radius);
        int apoR = (int)(a.apoAlt + body.radius);
        drawCircle(sx, sy, periR, {68, 68, 68, 255}, false);
        drawCircle(sx, sy, apoR, {68, 68, 68, 255}, false);

        Vec2 pp = cam.worldToScreen(a.periPoint, screenW, screenH);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect r1 = {(int)pp.x - 3, (int)pp.y - 3, 6, 6};
        SDL_RenderFillRect(renderer, &r1);
        char buf[32];
        snprintf(buf, sizeof(buf), "近 %.0f", a.periAlt);
        drawText(buf, (int)pp.x, (int)pp.y - 8, {170, 170, 170, 255}, fontSmall);

        Vec2 ap = cam.worldToScreen(a.apoPoint, screenW, screenH);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect r2 = {(int)ap.x - 3, (int)ap.y - 3, 6, 6};
        SDL_RenderFillRect(renderer, &r2);
        snprintf(buf, sizeof(buf), "远 %.0f", a.apoAlt);
        drawText(buf, (int)ap.x, (int)ap.y - 8, {170, 170, 170, 255}, fontSmall);
    }
}

void Renderer::drawMissionObjective(int screenW, int screenH) {
    drawText("任务目标：将飞行器从天体A变轨至天体B", screenW / 2, 24, {136, 136, 136, 255}, fontLarge);
    drawText("尽可能节省燃料 · 使轨道尽可能圆", screenW / 2, 42, {136, 136, 136, 255}, fontMedium);
}

void Renderer::drawText(const char* text, int x, int y, SDL_Color color, TTF_Font* font, bool center) {
    if (!font || !text || !text[0]) return;
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst;
    if (center) {
        dst = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    } else {
        dst = {x, y - surface->h / 2, surface->w, surface->h};
    }
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Renderer::drawCircle(int cx, int cy, int r, SDL_Color color, bool filled) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = -r; w <= r; w++) {
        for (int h = -r; h <= r; h++) {
            if (w * w + h * h <= r * r) {
                if (filled || w * w + h * h >= (r - 1) * (r - 1)) {
                    SDL_Rect p = {cx + w, cy + h, 1, 1};
                    SDL_RenderFillRect(renderer, &p);
                }
            }
        }
    }
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Renderer::drawDashedCircle(int cx, int cy, int r, SDL_Color color, int dashLen, int gapLen) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    float circ = 2 * (float)M_PI * r;
    int total = dashLen + gapLen;
    for (float a = 0; a < 360; a += 0.5f) {
        float rad = a * (float)M_PI / 180.0f;
        int seg = ((int)(a * r / 180.0f * (float)M_PI)) % total;
        if (seg < dashLen) {
            int x = cx + (int)(std::cos(rad) * r);
            int y = cy + (int)(std::sin(rad) * r);
            SDL_Rect p = {x, y, 1, 1};
            SDL_RenderFillRect(renderer, &p);
        }
    }
}

void Renderer::drawDashedLine(int x1, int y1, int x2, int y2, SDL_Color color, int dashLen, int gapLen) {
    int dx = x2 - x1, dy = y2 - y1;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1) return;
    float nx = dx / dist, ny = dy / dist;
    int total = dashLen + gapLen;
    for (float t = 0; t < dist; t += 1) {
        int seg = ((int)t) % total;
        if (seg < dashLen) {
            int x = x1 + (int)(nx * t);
            int y = y1 + (int)(ny * t);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_Rect p = {x, y, 1, 1};
            SDL_RenderFillRect(renderer, &p);
        }
    }
}
