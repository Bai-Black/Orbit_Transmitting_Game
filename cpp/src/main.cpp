#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <windows.h>
#include "game.h"
#include "input.h"
#include "renderer.h"
#include <cstdio>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF init failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Orbit Transfer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Input input;
    Renderer renderer(window, sdlRenderer);
    renderer.init();
    Game game(renderer, input);
    game.init();

    bool running = true;
    Uint32 lastTime = SDL_GetTicks();
    float accumulator = 0;

    while (running) {
        Uint32 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;
        if (delta > 0.1f) delta = 0.1f;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            input.handleEvent(e);

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_SPACE) {
                    if (game.getState() == GameState::PLAYING || game.getState() == GameState::PAUSED) {
                        game.togglePause();
                    } else if (game.getState() == GameState::TITLE ||
                               game.getState() == GameState::WIN ||
                               game.getState() == GameState::CRASH) {
                        game.reset();
                    }
                }
                if (e.key.keysym.sym == SDLK_F1) {
                    game.toggleDebug();
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                game.handlePauseClick(e.button.x, e.button.y);
            }
        }

        accumulator += delta;
        while (accumulator >= 1.0f / 60.0f) {
            game.update(1.0f / 60.0f);
            accumulator -= 1.0f / 60.0f;
        }

        renderer.clear();
        game.render();
        renderer.present();
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return main(__argc, __argv);
}
