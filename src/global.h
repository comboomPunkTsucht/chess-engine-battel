#pragma once

#include "nob_addons.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

inline const char *appDir = GetApplicationDirectory();
// Baut den Pfad relativ zur ausführbaren Datei zusammen
#ifdef __APPLE__
inline const char *assetPath = TextFormat("%s/../Resources/", appDir);
#else
inline const char *assetPath = TextFormat("%s/assets/", appDir);
#endif

#define MIN_SCREEN_WIDTH 1280
#define MIN_SCREEN_HEIGHT 720

#define DEFAULT_SCREEN_WIDTH MIN_SCREEN_WIDTH
#define DEFAULT_SCREEN_HEIGHT MIN_SCREEN_HEIGHT

inline uint32_t screen_width = DEFAULT_SCREEN_WIDTH;
inline uint32_t screen_height = DEFAULT_SCREEN_HEIGHT;

inline Texture2D chess_piece_textures;

inline size_t temp_mark;

static const char *setServerUri();

inline char *server_address = "0.0.0.0";
inline uint64_t server_port = 14318;
inline const char *server_uri = nullptr;

static const char *setServerUri() {
	const char *uri = TextFormat("ws://%s:%d", server_address, server_port);
	nob_log(INFO, "Websocket server URI: %s", uri);
	return uri;
}
