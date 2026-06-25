#pragma once
#include <SDL2/SDL.h>
#include <unordered_set>

class Input {
public:
    void handleEvent(SDL_Event& e);
    void update();
    bool isPressed(SDL_Scancode key) const;
    bool isActionPressed(const char* action) const;

private:
    std::unordered_set<SDL_Scancode> pressed;
};
