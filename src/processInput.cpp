#include "app.h"
#include "nob_addons.h"

void App::processMouseInput() {
	const Vector2 mouse = GetMousePosition();
	const int x = (int)mouse.x;
	const int y = (int)mouse.y;

	const bool down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
	const bool up = IsMouseButtonUp(MOUSE_BUTTON_LEFT);
	const bool pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	const bool released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

void App::processKeyboardInput() {
	if (IsKeyPressed(KEY_ESCAPE) || (IsKeyPressed(KEY_Q) && (IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER) || IsKeyDown(KEY_LEFT_CONTROL) ||
	                                                         IsKeyDown(KEY_RIGHT_CONTROL)))) {
		CloseWindow();
	}

	if (IsKeyPressed(KEY_F11)) {
		if (IsWindowFullscreen()) {
			ToggleFullscreen();
			SetWindowSize(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
		} else {
			ToggleFullscreen();
		}
	}

	if (IsKeyPressed(KEY_F10)) {
		vsync_enabled = !vsync_enabled;
		if (vsync_enabled) {
			SetWindowState(FLAG_VSYNC_HINT);
			targetFPS = GetMonitorRefreshRate(GetCurrentMonitor());
			SetTargetFPS(targetFPS);
		} else {
			ClearWindowState(FLAG_VSYNC_HINT);
			targetFPS = INT_MAX;
			SetTargetFPS(targetFPS);
		}
	}
}

void App::processInput() {
	processMouseInput();
	processKeyboardInput();
}
