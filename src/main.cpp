#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"

#define NOB_IMPLEMENTATION
#include "nob_addons.h"

#define FLAG_IMPLEMENTATION
#include "flag.h"

#define HT_IMPLEMENTATION
#include "ht.h"

int main(int argc, char **argv) {
  addon_init_logging();
  UNUSED(argc);
  UNUSED(argv);
  nob_log(INFO, "Hello, World!");

  // 1. Fenster und Umgebung initialisieren
  int screenWidth = 800;
  int screenHeight = 450;

  SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT |
                 FLAG_INTERLACED_HINT | FLAG_WINDOW_HIGHDPI |
                 FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(screenWidth, screenHeight, "Raylib Bouncing Circle");

  // 2. Variablen für den Kreis definieren
  Vector2 ballPosition = {(float)screenWidth / 2, (float)screenHeight / 2};
  Vector2 ballSpeed = {5.0f, 4.0f}; // Geschwindigkeit in X- und Y-Richtung
  int     ballRadius = 20;

  SetTargetFPS(
      GetMonitorRefreshRate(GetCurrentMonitor())); // Spiel-Logik auf 60 Frames
                                                   // pro Sekunde drosseln

  // 3. Haupt-Schleife (läuft, bis ESC gedrückt oder das Fenster geschlossen
  // wird)
  while (!WindowShouldClose()) {
    // --- UPDATE (Logik) ---

    if (IsWindowResized()) {
      screenWidth = GetScreenWidth();
      screenHeight = GetScreenHeight();
    }

    ballPosition.x += ballSpeed.x;
    ballPosition.y += ballSpeed.y;

    // Kollision mit der linken und rechten Fensterkante
    if ((ballPosition.x >= (screenWidth - ballRadius)) ||
        (ballPosition.x <= ballRadius)) {
      ballSpeed.x *= -1.0f; // Richtung auf der X-Achse umkehren
    }

    // Kollision mit der oberen und unteren Fensterkante
    if ((ballPosition.y >= (screenHeight - ballRadius)) ||
        (ballPosition.y <= ballRadius)) {
      ballSpeed.y *= -1.0f; // Richtung auf der Y-Achse umkehren
    }

    if (ballPosition.x < 0 || ballPosition.x > screenWidth ||
        ballPosition.y < 0 || ballPosition.y > screenHeight) {
      // Ball ist aus dem Fenster herausgefallen, zurücksetzen
      ballPosition.x = (float)screenWidth / 2;
      ballPosition.y = (float)screenHeight / 2;
    }

    // --- DRAW (Zeichnen) ---
    BeginDrawing();

    ClearBackground(
        RAYWHITE); // Bildschirm mit Farbe füllen (verhindert Schlieren)

    // Kreis zeichnen (Position, Radius, Farbe)
    DrawCircleV(ballPosition,
                (float)ballRadius * (min(screenWidth, screenHeight) * 0.1f),
                MAROON);

    // Optional: Einen Text in die obere linke Ecke setzen
    DrawText("Raylib Bouncing Circle", 10, 10, 20, DARKGRAY);

    DrawFPS(20, 40); // FPS in die obere linke Ecke setzen

    EndDrawing();
  }

  // 4. Aufräumen und Fenster schließen
  CloseWindow();

  return 0;
}
