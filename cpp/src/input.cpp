#include "input.h"

void Input::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        pressed.insert(e.key.keysym.scancode);
    } else if (e.type == SDL_KEYUP) {
        pressed.erase(e.key.keysym.scancode);
    }
}

void Input::update() {}

bool Input::isPressed(SDL_Scancode key) const {
    return pressed.count(key) > 0;
}

bool Input::isActionPressed(const char* action) const {
    if (action[0] == 'L') return isPressed(SDL_SCANCODE_LEFT) || isPressed(SDL_SCANCODE_A);
    if (action[0] == 'R') return isPressed(SDL_SCANCODE_RIGHT) || isPressed(SDL_SCANCODE_D);
    if (action[0] == 'U') return isPressed(SDL_SCANCODE_UP) || isPressed(SDL_SCANCODE_W);
    if (action[0] == 'D') return isPressed(SDL_SCANCODE_DOWN) || isPressed(SDL_SCANCODE_S);
    return false;
}
