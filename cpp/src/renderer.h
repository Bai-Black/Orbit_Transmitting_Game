#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "types.h"
#include "camera.h"
#include <vector>

class Renderer {
public:
    Renderer(SDL_Window* window, SDL_Renderer* renderer);
    ~Renderer();

    void init();
    void clear();
    void present();

    void drawBackground();
    void drawStars(const Camera& cam, int screenW, int screenH);
    void drawBody(const Body& body, const Camera& cam, int screenW, int screenH);
    void drawTrail(const std::vector<Vec2>& trail, const Camera& cam, int screenW, int screenH);
    void drawShip(const Ship& ship, const Camera& cam, int screenW, int screenH);
    void drawHUD(const Ship& ship, const Body* bodies, float fuelUsed, GameState state,
                 bool orbitStable, bool orbitReady, float orbitAngleTotal,
                 bool dominantIsB, const OrbitAnalysis* analysis, int screenW, int screenH);
    void drawTitle(int screenW, int screenH);
    void drawOverlay(const char* title, const char* subtitle, int score, float fuelUsed,
                     const OrbitAnalysis* analysis, int screenW, int screenH);
    void drawPause(int screenW, int screenH);
    void drawOffscreenIndicator(const Body& target, const Ship& ship, const Camera& cam,
                                int screenW, int screenH);
    void drawOrbitPrediction(const OrbitData* data, const Body& body, bool orbitStable,
                             const Camera& cam, int screenW, int screenH);
    void drawMissionObjective(int screenW, int screenH);

    void generateStars();
    void getWindowSize(int& w, int& h);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* fontSmall = nullptr;
    TTF_Font* fontMedium = nullptr;
    TTF_Font* fontLarge = nullptr;
    TTF_Font* fontTitle = nullptr;

    struct Star { float x, y, brightness; };
    std::vector<Star> stars;
    SDL_Texture* starTexture = nullptr;

    // 文本对齐：0=居中, 1=左对齐, 2=右对齐
    void drawText(const char* text, int x, int y, SDL_Color color, TTF_Font* font, int align = 0);
    void drawCircle(int cx, int cy, int r, SDL_Color color, bool filled = false);
    void drawLine(int x1, int y1, int x2, int y2, SDL_Color color);
    void drawDashedCircle(int cx, int cy, int r, SDL_Color color, int dashLen, int gapLen);
    void drawDashedLine(int x1, int y1, int x2, int y2, SDL_Color color, int dashLen, int gapLen);
};
