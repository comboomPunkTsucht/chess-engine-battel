#pragma once

#ifdef __APPLE__
#define ASSETS_PATH "../Resources/"
#else
#define ASSETS_PATH "assets/"
#endif

#include "raylib.h"
#include <algorithm>
#include <cmath>

#include <cstdint>

constexpr uint32_t DEFAULT_SCREEN_WIDTH = 800;
constexpr uint32_t DEFAULT_SCREEN_HEIGHT = 450;
inline uint32_t	   screen_width = DEFAULT_SCREEN_WIDTH;
inline uint32_t	   screen_height = DEFAULT_SCREEN_HEIGHT;

static float getFontSize();

inline Texture2D chess_piece_textures;

inline Font		 main_font;
inline Font		 bold_font;
inline Font		 italic_font;
inline size_t	 temp_mark;

inline float	 font_size_factor = 0.044f;

inline float	 font_size = getFontSize();

static float getFontSize() { return std::max(std::floor(screen_width * font_size_factor + 0.2), std::floor(screen_height * 0.025)); }
