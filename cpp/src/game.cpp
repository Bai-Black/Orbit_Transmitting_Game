#include "game.h"
#include "physics.h"
#include "orbit.h"
#include <cmath>
#include <algorithm>

Game::Game(Renderer& renderer, Input& input) : renderer(renderer), input(input) {
    bodies[0] = {{300, 400}, 5000, 35, "天体 A"};
    bodies[1] = {{1200, 400}, 3000, 25, "天体 B"};
}

void Game::init() {
    reset();
}

void Game::reset() {
    Body& b = bodies[0];
    float r = b.radius + ORBIT_R_OFFSET;
    float v = std::sqrt(G * b.mass / r);
    ship.pos = {b.pos.x + r, b.pos.y};
    ship.vel = {0, -v};
    ship.angle = -3.14159f / 2.0f;
    ship.throttle = 0;
    trail.clear();
    fuelUsed = 0;
    totalScore = 0;
    state = GameState::PLAYING;
    orbitReady = false;
    orbitAngleStart = 0;
    orbitAngleTotal = 0;
    orbitStable = false;
    stabilityConfirmCount = 0;
    lastPeriAlt = -1;
    lastApoAlt = -1;
    orbitStableA = false;
    stabilityConfirmCountA = 0;
    lastPeriAltA = -1;
    lastApoAltA = -1;
    dominantBody = &bodies[0];
    prevDominantBody = &bodies[0];
    accumulator = 0;
    camera.x = ship.pos.x;
    camera.y = ship.pos.y;
    delete orbitData;
    orbitData = nullptr;
}

void Game::update(float dt) {
    if (state != GameState::PLAYING) return;

    // Input
    if (input.isActionPressed("L")) ship.angle -= ROT_SPEED * dt;
    if (input.isActionPressed("R")) ship.angle += ROT_SPEED * dt;
    if (input.isActionPressed("U")) {
        ship.throttle = std::min(1.0f, ship.throttle + 1.5f * dt);
    } else if (input.isActionPressed("D")) {
        ship.throttle = std::max(0.0f, ship.throttle - 1.5f * dt);
    } else {
        ship.throttle = std::max(0.0f, ship.throttle - 0.3f * dt);
    }

    // SOI detection
    float minDist = 1e30f;
    Body* closest = &bodies[0];
    for (int i = 0; i < 2; i++) {
        float dx = ship.pos.x - bodies[i].pos.x;
        float dy = ship.pos.y - bodies[i].pos.y;
        float d = std::sqrt(dx * dx + dy * dy);
        if (d < minDist) { minDist = d; closest = &bodies[i]; }
    }

    if (closest != prevDominantBody) {
        float dA = std::sqrt((ship.pos.x - bodies[0].pos.x) * (ship.pos.x - bodies[0].pos.x) +
                             (ship.pos.y - bodies[0].pos.y) * (ship.pos.y - bodies[0].pos.y));
        float dB = std::sqrt((ship.pos.x - bodies[1].pos.x) * (ship.pos.x - bodies[1].pos.x) +
                             (ship.pos.y - bodies[1].pos.y) * (ship.pos.y - bodies[1].pos.y));
        float boundary = (dA + dB) / 2.0f;
        if (closest == &bodies[0] && dA < boundary * 0.8f) prevDominantBody = &bodies[0];
        else if (closest == &bodies[1] && dB < boundary * 0.8f) prevDominantBody = &bodies[1];
    } else {
        prevDominantBody = closest;
    }
    dominantBody = prevDominantBody;

    // Gravity
    Vec2 g = physics::gravity(ship.pos, dominantBody->pos, dominantBody->mass, G);
    Vec2 accel = g;

    // Thrust
    if (ship.throttle > 0.01f) {
        accel.x += THRUST_MAX * ship.throttle * std::cos(ship.angle);
        accel.y += THRUST_MAX * ship.throttle * std::sin(ship.angle);
    }

    fuelUsed += ship.throttle * dt;
    physics::integrate(ship, accel, dt);

    // Trail
    trail.push_back(ship.pos);
    if ((int)trail.size() > TRAIL_LEN) trail.erase(trail.begin());

    // Crash
    for (int i = 0; i < 2; i++) {
        float dx = ship.pos.x - bodies[i].pos.x;
        float dy = ship.pos.y - bodies[i].pos.y;
        if (dx * dx + dy * dy < bodies[i].radius * bodies[i].radius) {
            state = GameState::CRASH;
            return;
        }
    }

    // Orbit B
    float dx2 = ship.pos.x - bodies[1].pos.x;
    float dy2 = ship.pos.y - bodies[1].pos.y;
    float dist2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
    float orbitR = dist2 - bodies[1].radius;

    if (dominantBody == &bodies[1] && orbitR > ORBIT_MIN_VAL && orbitR < ORBIT_MAX_VAL) {
        if (!orbitStable) {
            auto pts = orbit::predict(ship.pos, ship.vel, bodies[1], G, FIXED_DT);
            auto analysis = orbit::analyze(ship, bodies[1], G, FIXED_DT);
            delete orbitData;
            orbitData = new OrbitData{pts, analysis};
        }

        const OrbitAnalysis* analysis = orbitData ? &orbitData->analysis : nullptr;

        if (ship.throttle < 0.01f && analysis) {
            bool periOk = lastPeriAlt < 0 || std::abs(analysis->periAlt - lastPeriAlt) < 15;
            bool apoOk = lastApoAlt < 0 || std::abs(analysis->apoAlt - lastApoAlt) < 15;
            lastPeriAlt = analysis->periAlt;
            lastApoAlt = analysis->apoAlt;

            if (periOk && apoOk && analysis->closed) {
                stabilityConfirmCount++;
                if (stabilityConfirmCount >= STABILITY_CONFIRM_FRAMES) orbitStable = true;
            } else {
                stabilityConfirmCount = std::max(0, stabilityConfirmCount - 1);
                if (stabilityConfirmCount == 0) orbitStable = false;
            }
        } else if (ship.throttle >= 0.01f) {
            orbitStable = false;
            stabilityConfirmCount = 0;
            lastPeriAlt = -1;
            lastApoAlt = -1;
        }

        if (analysis && analysis->periAlt > TARGET_ORBIT_MIN && analysis->apoAlt < TARGET_ORBIT_MAX && orbitStable) {
            if (!orbitReady) {
                orbitReady = true;
                orbitAngleStart = std::atan2(ship.pos.y - bodies[1].pos.y, ship.pos.x - bodies[1].pos.x);
                orbitAngleTotal = 0;
            } else {
                float cur = std::atan2(ship.pos.y - bodies[1].pos.y, ship.pos.x - bodies[1].pos.x);
                float dA = cur - orbitAngleStart;
                if (dA > 3.14159f) dA -= 2 * 3.14159f;
                if (dA < -3.14159f) dA += 2 * 3.14159f;
                orbitAngleTotal += dA;
                orbitAngleStart = cur;
                if (std::abs(orbitAngleTotal) >= 2 * 3.14159f) {
                    const auto& a = orbitData->analysis;
                    float fuelScore = std::max(0.0f, 500.0f - fuelUsed * 50);
                    float circ = (a.apoAlt + a.periAlt > 0) ? 1.0f - (a.apoAlt - a.periAlt) / (a.apoAlt + a.periAlt) : 0;
                    float circScore = std::max(0.0f, circ * 500);
                    totalScore = (int)std::round(fuelScore + circScore);
                    state = GameState::WIN;
                    return;
                }
            }
        } else {
            orbitReady = false;
            orbitAngleTotal = 0;
        }
    } else {
        delete orbitData;
        orbitData = nullptr;
        orbitReady = false;
        orbitAngleTotal = 0;

        if (dominantBody == &bodies[0]) {
            float dA = std::sqrt((ship.pos.x - bodies[0].pos.x) * (ship.pos.x - bodies[0].pos.x) +
                                 (ship.pos.y - bodies[0].pos.y) * (ship.pos.y - bodies[0].pos.y));
            float oRA = dA - bodies[0].radius;

            if (ship.throttle < 0.01f && oRA > ORBIT_MIN_VAL && oRA < ORBIT_MAX_VAL) {
                auto aA = orbit::analyze(ship, bodies[0], G, FIXED_DT);
                bool pOk = lastPeriAltA < 0 || std::abs(aA.periAlt - lastPeriAltA) < 15;
                bool aOk = lastApoAltA < 0 || std::abs(aA.apoAlt - lastApoAltA) < 15;
                lastPeriAltA = aA.periAlt;
                lastApoAltA = aA.apoAlt;
                if (pOk && aOk && aA.closed) {
                    stabilityConfirmCountA++;
                    if (stabilityConfirmCountA >= STABILITY_CONFIRM_FRAMES) orbitStableA = true;
                } else {
                    stabilityConfirmCountA = std::max(0, stabilityConfirmCountA - 1);
                    if (stabilityConfirmCountA == 0) orbitStableA = false;
                }
            } else if (ship.throttle >= 0.01f) {
                orbitStableA = false;
                stabilityConfirmCountA = 0;
                lastPeriAltA = -1;
                lastApoAltA = -1;
            }
        } else {
            orbitStableA = false;
            stabilityConfirmCountA = 0;
            lastPeriAltA = -1;
            lastApoAltA = -1;
        }
    }

    // Escape
    bool tooFar = true;
    for (int i = 0; i < 2; i++) {
        float ddx = ship.pos.x - bodies[i].pos.x;
        float ddy = ship.pos.y - bodies[i].pos.y;
        if (ddx * ddx + ddy * ddy < ESCAPE_DIST * ESCAPE_DIST) { tooFar = false; break; }
    }
    if (tooFar) reset();
}

void Game::render() {
    int w, h;
    SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &w, &h);

    if (state == GameState::TITLE) {
        renderer.drawTitle(w, h);
        return;
    }

    camera.update(ship.pos, 0.1f, w, h);

    renderer.drawBackground();
    renderer.drawStars(camera, w, h);
    renderer.drawTrail(trail, camera, w, h);
    for (int i = 0; i < 2; i++) renderer.drawBody(bodies[i], camera, w, h);
    renderer.drawOrbitPrediction(orbitData, bodies[1], orbitStable, camera, w, h);
    renderer.drawShip(ship, camera, w, h);
    renderer.drawHUD(ship, bodies, fuelUsed, state, orbitStable, orbitReady, orbitAngleTotal, w, h);
    renderer.drawOffscreenIndicator(bodies[1], ship, camera, w, h);
    renderer.drawMissionObjective(w, h);

    if (state == GameState::WIN)
        renderer.drawOverlay("任 务 完 成", "成功变轨至目标天体", totalScore, fuelUsed, orbitData ? &orbitData->analysis : nullptr, w, h);
    if (state == GameState::CRASH)
        renderer.drawOverlay("坠 毁", "飞行器撞击天体表面", 0, 0, nullptr, w, h);
    if (state == GameState::PAUSED)
        renderer.drawPause(w, h);
}

void Game::togglePause() {
    if (state == GameState::PLAYING) state = GameState::PAUSED;
    else if (state == GameState::PAUSED) state = GameState::PLAYING;
}

void Game::toggleDebug() { debugMode = !debugMode; }

bool Game::handlePauseClick(int mx, int my) {
    if (state != GameState::PAUSED) return false;
    int w, h;
    SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &w, &h);
    int bw = 180, bh = 40, cx = w / 2;
    int cy1 = h / 2 - 10, cy2 = h / 2 + 50;

    if (mx >= cx - bw / 2 && mx <= cx + bw / 2) {
        if (my >= cy1 && my <= cy1 + bh) { state = GameState::PLAYING; return true; }
        if (my >= cy2 && my <= cy2 + bh) { reset(); return true; }
    }
    return false;
}
