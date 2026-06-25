#pragma once
#include "types.h"
#include "input.h"
#include "camera.h"
#include "renderer.h"
#include <vector>

class Game {
public:
    Game(Renderer& renderer, Input& input);

    void init();
    void reset();
    void update(float dt);
    void render();

    GameState getState() const { return state; }
    void togglePause();
    void toggleDebug();

    // For click handling
    bool handlePauseClick(int mx, int my);

private:
    Renderer& renderer;
    Input& input;

    static constexpr float G = 500.0f;
    static constexpr float THRUST_MAX = 200.0f;
    static constexpr float ROT_SPEED = 2.5f;
    static constexpr float FIXED_DT = 1.0f / 60.0f;
    static constexpr float ORBIT_R_OFFSET = 120.0f;
    static constexpr float TARGET_ORBIT_MIN = 60.0f;
    static constexpr float TARGET_ORBIT_MAX = 300.0f;
    static constexpr float ORBIT_MIN_VAL = 0.0f;
    static constexpr float ORBIT_MAX_VAL = 1000.0f;
    static constexpr float ESCAPE_DIST = 3000.0f;
    static constexpr int STABILITY_CONFIRM_FRAMES = 5;
    static constexpr int TRAIL_LEN = 300;

    Body bodies[2];
    Ship ship;
    GameState state = GameState::TITLE;
    Camera camera;
    std::vector<Vec2> trail;

    float fuelUsed = 0;
    int totalScore = 0;

    // Orbit B
    OrbitData* orbitData = nullptr;
    bool orbitReady = false;
    float orbitAngleStart = 0;
    float orbitAngleTotal = 0;
    bool orbitStable = false;
    int stabilityConfirmCount = 0;
    float lastPeriAlt = -1;
    float lastApoAlt = -1;

    // Orbit A
    bool orbitStableA = false;
    int stabilityConfirmCountA = 0;
    float lastPeriAltA = -1;
    float lastApoAltA = -1;

    // SOI
    Body* dominantBody = &bodies[0];
    Body* prevDominantBody = &bodies[0];

    bool debugMode = false;
    float accumulator = 0;

    // Pause buttons
    Button pauseBtns[2];
};
