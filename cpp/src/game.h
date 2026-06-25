#pragma once
#include "types.h"
#include "input.h"
#include "camera.h"
#include "renderer.h"
#include <vector>

// 游戏核心逻辑：状态管理、物理更新、胜利判定
class Game {
public:
    Game(Renderer& renderer, Input& input);

    void init();        // 初始化
    void reset();       // 重置游戏
    void update(float dt); // 物理+逻辑更新
    void render();      // 渲染

    GameState getState() const { return state; }
    void togglePause();
    void toggleDebug();
    bool handlePauseClick(int mx, int my);

private:
    Renderer& renderer;
    Input& input;

    // 物理常量
    static constexpr float G = 500.0f;          // 引力常数
    static constexpr float THRUST_MAX = 200.0f; // 最大推力
    static constexpr float ROT_SPEED = 2.5f;    // 旋转速度
    static constexpr float FIXED_DT = 1.0f / 60.0f; // 固定时间步长
    static constexpr float ORBIT_R_OFFSET = 120.0f; // 初始轨道偏移
    static constexpr float TARGET_ORBIT_MIN = 60.0f; // 目标轨道最小高度
    static constexpr float TARGET_ORBIT_MAX = 300.0f; // 目标轨道最大高度
    static constexpr float ORBIT_MIN_VAL = 0.0f;  // 轨道检测最小范围
    static constexpr float ORBIT_MAX_VAL = 1000.0f; // 轨道检测最大范围
    static constexpr float ESCAPE_DIST = 3000.0f;  // 逃逸距离
    static constexpr int STABILITY_CONFIRM_FRAMES = 5; // 稳定性确认帧数
    static constexpr int TRAIL_LEN = 300;           // 轨迹长度

    Body bodies[2];     // 两个天体
    Ship ship;          // 飞行器
    GameState state = GameState::TITLE;
    Camera camera;
    std::vector<Vec2> trail; // 飞行轨迹

    float fuelUsed = 0;   // 燃料消耗量
    int totalScore = 0;   // 总分

    // 天体B轨道状态
    OrbitData* orbitData = nullptr;
    bool orbitReady = false;
    float orbitAngleStart = 0;
    float orbitAngleTotal = 0;
    bool orbitStable = false;
    int stabilityConfirmCount = 0;
    float lastPeriAlt = -1;
    float lastApoAlt = -1;

    // 天体A轨道状态
    bool orbitStableA = false;
    int stabilityConfirmCountA = 0;
    float lastPeriAltA = -1;
    float lastApoAltA = -1;

    // 引力范围（SOI）检测
    Body* dominantBody = &bodies[0];
    Body* prevDominantBody = &bodies[0];

    bool debugMode = false;
    float accumulator = 0;

    Button pauseBtns[2];
};
