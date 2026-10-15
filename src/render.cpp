#include "app.h"
#include "game.h"
#include "global.h"
#include "nob_addons.h"
#include "piece.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <math.h>

#include "nord.h"

void App::render() { mCurrentGame->Game::render(); }

void Game::render() {
	const BoardLayout layout = Game::computeLayout();
	const int space_size = layout.space_size;
	const int x_start = layout.x_start;
	const int y_start = layout.y_start;

	const float piece_size = space_size * 0.7f;

	BeginDrawing();
	ClearBackground(NORD_BACKGROUND_COLOR);
	DrawTextEx(main_font, temp_sprintf("%d FPS", GetFPS()), (Vector2){1, 1}, font_size, font_size / 10.0, NORD_PRIMARY_COLOR);

	DrawRectangleLines(x_start - 1, y_start - 1, DEFAULT_BOARD_DIMENSIONS * space_size + 2, DEFAULT_BOARD_DIMENSIONS * space_size + 2,
	                   NORD_FOREGROUND_COLOR);

	for (uint32_t x = 0; x < DEFAULT_BOARD_DIMENSIONS; x++) {
		for (uint32_t y = 0; y < DEFAULT_BOARD_DIMENSIONS; y++) {
			float px = x * space_size + x_start;
			float py = y * space_size + y_start;

			if ((x + y) % 2 == 0) { DrawRectangle(px, py, space_size, space_size, get_Nord_Color_as_raylib_color(NORD1)); }

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
						DrawTexturePro(chess_piece_textures, sourceRec, destRec, (Vector2){0, 0}, // Ursprung (oben links relativ zu destRec)
						               0.0f,                                                      // Keine Rotation
						               get_Nord_Color_as_raylib_color(NORD4));
					}
					if (piece.color == PLAYER_BLACK) {
						DrawTexturePro(chess_piece_textures, sourceRec, destRec, (Vector2){0, 0}, // Ursprung (oben links relativ zu destRec)
						               0.0f,                                                      // Keine Rotation
						               get_Nord_Color_as_raylib_color(NORD3));
					}

					break;
			}
		}
	}
	// DrawTexture(game->pieces, 1, 1, NORD_FOREGROUND_COLOR);
	EndDrawing();
}
