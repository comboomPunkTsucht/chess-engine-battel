#include "app.h"
#include "game.h"

#include "nob_addons.h"
#include "piece.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <math.h>

#include "nord.h"

void App::render() { mCurrentGame->Game::render(this); }

void Game::render(App *app) {
	const BoardLayout layout = Game::computeLayout(app);
	const int space_size = layout.space_size;
	const int x_start = layout.x_start;
	const int y_start = layout.y_start;

	const float piece_size = floor(space_size * 0.7f);

	BeginDrawing();
	ClearBackground(NORD_BACKGROUND_COLOR);

	DrawRectangleLines(x_start - 1, y_start - 1, DEFAULT_BOARD_DIMENSIONS * space_size + 2, DEFAULT_BOARD_DIMENSIONS * space_size + 2,
	                   NORD_FOREGROUND_COLOR);

	for (uint32_t x = 0; x < DEFAULT_BOARD_DIMENSIONS; x++) {
		for (uint32_t y = 0; y < DEFAULT_BOARD_DIMENSIONS; y++) {
			float px = x * space_size + x_start;
			float py = y * space_size + y_start;

			if ((x + y) % 2 == 0) {
				DrawRectangle(px, py, space_size, space_size, get_Nord_Color_as_raylib_color(NORD1));
			} else {
				DrawRectangle(px, py, space_size, space_size, NORD_BACKGROUND_COLOR);
			}

			uint32_t idx = x + y * DEFAULT_BOARD_DIMENSIONS;
			Piece piece = board[idx];

			// 1. Definiere den Ausschnitt auf dem Sprite-Sheet (unverändert)
			Rectangle sourceRec = {(float)(((float)piece.type - 1.0) * 20.0), 19.0f, 20.0f, 19.0f};

			// 2. Berechne den Offset, um das kleinere Piece mittig im Feld zu
			// zentrieren
			float center_offset = (space_size - piece_size) / 2.0f;

			// 3. Definiere Position und finale skalierte Größe auf dem
			// Bildschirm
			Rectangle destRec = {px + center_offset, py + center_offset, piece_size, piece_size};

			switch (piece.type) {
				case None: break;
				default:
					if (piece.color == PLAYER_WHITE) {
						DrawTexturePro(app->chess_piece_textures, sourceRec, destRec, (Vector2){0, 0}, // Ursprung (oben links relativ zu destRec)
						               0.0f,                                                           // Keine Rotation
						               get_Nord_Color_as_raylib_color(NORD4));
					}
					if (piece.color == PLAYER_BLACK) {
						DrawTexturePro(app->chess_piece_textures, sourceRec, destRec, (Vector2){0, 0}, // Ursprung (oben links relativ zu destRec)
						               0.0f,                                                           // Keine Rotation
						               get_Nord_Color_as_raylib_color(NORD3));
					}

					break;
			}
		}
	}
	// DrawTexture(game->pieces, 1, 1, NORD_FOREGROUND_COLOR);

	DrawTextEx(App::get_font(App::FONT_REGULAR, 20.0f), temp_sprintf("%d FPS", GetFPS()), (Vector2){1, 1}, App::getFontSize(20.0f),
	           App::getFontSize(20.0f) / 10.0, NORD_PRIMARY_COLOR);

	DrawTextEx(App::get_font(App::FONT_REGULAR, 20.0f), temp_sprintf("Target FPS: %d", app->targetFPS),
	           (Vector2){1, 1 + App::getFontSize(20.0f) + 10}, App::getFontSize(20.0f), App::getFontSize(20.0f) / 10.0, NORD_PRIMARY_COLOR);

	DrawTextEx(App::get_font(App::FONT_REGULAR, 20.0f), temp_sprintf("VSync: %s", app->vsync_enabled ? "ON" : "OFF"),
	           (Vector2){1, 1 + 2 * App::getFontSize(20.0f) + 20}, App::getFontSize(20.0f), App::getFontSize(20.0f) / 10.0, NORD_PRIMARY_COLOR);
	EndDrawing();
}
