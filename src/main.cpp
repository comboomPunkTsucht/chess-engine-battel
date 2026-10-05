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
  Vector2 ballSpeed = {screenWidth * 0.01f,
                       screenHeight *
                           0.01f}; // Geschwindigkeit in X- und Y-Richtung
  float   ballRadius = font_size * 0.5;

  float   updateRate = 20.0f;
  float   timePerTick = 1.0f / updateRate;
  float   timeAccumulator = 0.0f;

  Vector2 previousWindowPosition = GetWindowPosition();

  bool    debug = false; // <-- Setze dies auf true, um Debugging zu aktivieren

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

    if (IsKeyPressed(KEY_F3)) {
      debug = !debug;
      nob_log(INFO, "Debugging %s", debug ? "enabled" : "disabled");
    }

    // Zeit seit dem letzten Frame zum Accumulator hinzufügen
    timeAccumulator += GetFrameTime();

    // Logik in festen Zeitschritten (20-mal pro Sekunde) ausführen
    // Logik in festen Zeitschritten (20-mal pro Sekunde) ausführen
    while (timeAccumulator >= timePerTick) {

      previousBallPosition = ballPosition;

      // NEU: Fensterbewegung erfassen und auf den Ball übertragen
      Vector2 currentWindowPosition = GetWindowPosition();
      Vector2 windowDelta = {currentWindowPosition.x - previousWindowPosition.x,
                             currentWindowPosition.y -
                                 previousWindowPosition.y};

      // Fensterposition für den nächsten Tick aktualisieren
      previousWindowPosition = currentWindowPosition;

      // Wenn das Fenster bewegt wurde, addieren wir einen Bruchteil des Deltas
      // zur Ballgeschwindigkeit. Der Ball bewegt sich *entgegen* der
      // Fensterbewegung (Trägheit). Faktor anpassen, um den Effekt zu
      // verstärken oder abzuschwächen.
      if (windowDelta.x != 0.0f || windowDelta.y != 0.0f) {
        ballSpeed.x -= windowDelta.x * 0.15f;
        ballSpeed.y -= windowDelta.y * 0.15f;
      }

      // 1. Schwerkraft anwenden: Die Y-Geschwindigkeit wird in jedem Tick
      // erhöht
      float gravity = 0.002f * ballRadius; // Stärke der Schwerkraft anpassen
                                           // je nach gewünschtem "Gewicht"
      ballSpeed.y += gravity;

      // Position aktualisieren
      ballPosition += ballSpeed;

      // 2. Kollision mit der linken und rechten Fensterkante
      if (ballPosition.x >= (screenWidth - ballRadius)) {
        ballPosition.x =
            screenWidth - ballRadius; // Verhindert Feststecken in der Wand
        ballSpeed.x *= -0.8f;         // Abprallen mit Energieverlust (Dämpfung)
      } else if (ballPosition.x <= ballRadius) {
        ballPosition.x = ballRadius;
        ballSpeed.x *= -0.8f;
      }

      // 3. Kollision mit der UNTEREN Fensterkante (Boden)
      if (ballPosition.y >= (screenHeight - ballRadius)) {
        ballPosition.y =
            screenHeight - ballRadius; // Verhindert Einsinken in den Boden

        // Energieverlust beim Aufprall
        ballSpeed.y *= -0.75f;

        // Bodenreibung: Horizontale Geschwindigkeit verringern, wenn der Ball
        // rollt/rutscht
        ballSpeed.x *= 0.95f;

        // Ruhezustand: Wenn der Ball am Boden ist und nur noch minimal hüpft,
        // stoppe ihn komplett auf der Y-Achse
        if (fabs(ballSpeed.y) < 3.0f) { ballSpeed.y = 0.0f; }

        // Wenn er auch fast nicht mehr rollt, stoppe ihn komplett auf der
        // X-Achse
        if (fabs(ballSpeed.x) < 0.5f) { ballSpeed.x = 0.0f; }
      }
      // 4. Kollision mit der OBEREN Fensterkante (Decke)
      else if (ballPosition.y <= ballRadius) {
        ballPosition.y = ballRadius;
        ballSpeed.y *= -0.8f;
      }

      // 5. Fallback: Ball ist komplett aus dem Fenster gefallen (z.B. beim
      // Verkleinern des Fensters)
      if (ballPosition.x < -100 || ballPosition.x > screenWidth + 100 ||
          ballPosition.y < -100 || ballPosition.y > screenHeight + 100) {
        ballPosition.x = (float)screenWidth / 2;
        ballPosition.y = (float)screenHeight / 2;
        ballSpeed = {screenWidth * 0.01f,
                     screenHeight * 0.01f}; // Neustart-Schwung
      }

      // Die konsumierte Zeit vom Accumulator abziehen
      timeAccumulator -= timePerTick;
    }

    float   alpha = timeAccumulator / timePerTick;

    Vector2 renderPosition =
        Vector2Lerp(previousBallPosition, ballPosition, alpha);

    // breakpoint(); // <-- Hier wird der Breakpoint gesetzt, um den Ball zu
    //  inspizieren
    // --- DRAW (Zeichnen) ---
    BeginDrawing();

    ClearBackground(NORD_BACKGROUND_COLOR); // Bildschirm mit Farbe füllen
                                            // (verhindert Schlieren)

    // Kreis zeichnen (Position, Radius, Farbe)
    DrawCircleV(renderPosition, ballRadius, NORD_PRIMARY_COLOR);

    if (debug) {
      DrawTextEx(font_regular,
                 nob_temp_sprintf("Ball Position: (%.2f, %.2f)",
                                  renderPosition.x, renderPosition.y),
                 {10, 58}, 20, 4, NORD_INFO_COLOR);
      DrawTextEx(font_regular,
                 nob_temp_sprintf("Ball Speed: (%.2f, %.2f)", ballSpeed.x,
                                  ballSpeed.y),
                 {10, 82}, 20, 4, NORD_INFO_COLOR);

      // Optional: Einen Text in die obere linke Ecke setzen
      DrawTextEx(font_regular, "Raylib Bouncing Circle", {10, 10}, 20, 4,
                 NORD_FOREGROUND_COLOR);

      DrawTextEx(font_regular, nob_temp_sprintf("%d FPS", GetFPS()), {10, 34},
                 20, 4, NORD_PRIMARY_COLOR);
    }
    EndDrawing();
  }

  // 4. Aufräumen und Fenster schließen
  CloseWindow();

  return 0;
}
