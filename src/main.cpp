#include "assets.h"
#include <algorithm>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"

#include "raygui.h"

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
  int   screenWidth = 800;
  int   screenHeight = 450;
  float font_size =
      std::max(floor(screenWidth * 0.044 + 0.2), floor(screenHeight * 0.025));

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT |
                 FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE |
                 FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(screenWidth, screenHeight, "Raylib Bouncing Circle");

  Font font_regular = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Regular_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Regular_ttf_size, font_size, NULL, 0);
  Font font_itallic = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Italic_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Italic_ttf_size, font_size, NULL, 0);

  Font font_bold = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Bold_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Bold_ttf_size, font_size, NULL, 0);

#ifndef __APPLE__

  SetWindowIcon(
      LoadImageFromMemory(".png", Assets::icon_png, Assets::icon_png_size));

#endif
  // 2. Variablen für den Kreis definieren
  // 2. Variablen für den Kreis definieren
  Vector2 ballPosition = {(float)screenWidth / 2, (float)screenHeight / 2};
  Vector2 previousBallPosition =
      ballPosition; // NEU: Speichert die Position vor dem Update
  Vector2 ballSpeed = {5.0f, 4.0f}; // Geschwindigkeit in X- und Y-Richtung
  float   ballRadius = font_size * 0.5;

  float   updateRate = 20.0f;
  float   timePerTick = 1.0f / updateRate;
  float   timeAccumulator = 0.0f;

  SetTargetFPS(
      GetMonitorRefreshRate(GetCurrentMonitor())); // Spiel-Logik auf 60 Frames
                                                   // pro Sekunde drosseln

  // 3. Haupt-Schleife (läuft, bis ESC gedrückt oder das Fenster geschlossen
  // wird)
  size_t mark = temp_save();
  while (!WindowShouldClose()) {
    temp_rewind(mark); // Alle temporären Strings zurücksetzen
    // --- UPDATE (Logik) ---

    if (IsWindowResized()) {
      screenWidth = GetScreenWidth();
      screenHeight = GetScreenHeight();

      font_size = std::max(floor(screenWidth * 0.044 + 0.2),
                           floor(screenHeight * 0.025)) < 20
                      ? 20
                      : std::max(floor(screenWidth * 0.044 + 0.2),
                                 floor(screenHeight * 0.025));
      ballRadius = font_size * 0.5;
      font_regular = LoadFontFromMemory(
          ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Regular_ttf,
          Assets::CaskaydiaCoveNerdFontPropo_Regular_ttf_size, font_size, NULL,
          0);
      font_itallic = LoadFontFromMemory(
          ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Italic_ttf,
          Assets::CaskaydiaCoveNerdFontPropo_Italic_ttf_size, font_size, NULL,
          0);

      font_bold = LoadFontFromMemory(
          ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Bold_ttf,
          Assets::CaskaydiaCoveNerdFontPropo_Bold_ttf_size, font_size, NULL, 0);
    }

    // Zeit seit dem letzten Frame zum Accumulator hinzufügen
    timeAccumulator += GetFrameTime();

    // Logik in festen Zeitschritten (20-mal pro Sekunde) ausführen
    while (timeAccumulator >= timePerTick) {
      previousBallPosition = ballPosition;

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

      // Die konsumierte Zeit vom Accumulator abziehen
      timeAccumulator -= timePerTick;
    }

    float   alpha = timeAccumulator / timePerTick;

    Vector2 renderPosition =
        Vector2Lerp(previousBallPosition, ballPosition, alpha);

    breakpoint(); // <-- Hier wird der Breakpoint gesetzt, um den Ball zu
                  // inspizieren
    // --- DRAW (Zeichnen) ---
    BeginDrawing();

    ClearBackground(NORD_BACKGROUND_COLOR); // Bildschirm mit Farbe füllen
                                            // (verhindert Schlieren)

    // Kreis zeichnen (Position, Radius, Farbe)
    DrawCircleV(renderPosition, ballRadius, NORD_PRIMARY_COLOR);

    // Optional: Einen Text in die obere linke Ecke setzen
    DrawTextEx(font_regular, "Raylib Bouncing Circle", {10, 10}, 20, 4,
               NORD_FOREGROUND_COLOR);

    DrawTextEx(font_regular, nob_temp_sprintf("%d FPS", GetFPS()), {10, 34}, 20,
               4, NORD_PRIMARY_COLOR);

    EndDrawing();
  }

  // 4. Aufräumen und Fenster schließen
  CloseWindow();

  return 0;
}
