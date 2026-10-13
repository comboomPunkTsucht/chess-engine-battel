#include "game.h"

void Game::processInput() {
    const Vector2 mouse = GetMousePosition();
    const int x = (int) mouse.x;
    const int y = (int) mouse.y;

    const bool down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    const bool up = IsMouseButtonUp(MOUSE_BUTTON_LEFT);
    const bool pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    const bool released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}