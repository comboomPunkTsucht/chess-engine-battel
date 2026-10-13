#ifndef NORD_COLORS_H
#define NORD_COLORS_H

typedef enum {
  // Polar Night
  NORD0 = 0x2e3440ff,
  NORD1 = 0x3b4252ff,
  NORD2 = 0x434c5eff,
  NORD3 = 0x4c566aff,
  // Snow Storm
  NORD4 = 0xd8dee9ff,
  NORD5 = 0xe5e9f0ff,
  NORD6 = 0xeceff4ff,
  // Frost
  NORD7 = 0x8fbcbbff,
  NORD8 = 0x88c0d0ff,
  NORD9 = 0x81a1c1ff,
  NORD10 = 0x5e81acff,
  // Aurora
  NORD11 = 0xbf616aff,
  NORD12 = 0xd08770ff,
  NORD13 = 0xebcb8bff,
  NORD14 = 0xa3be8cff,
  NORD15 = 0xb48eadff,

} Nord_Colors;

static const Nord_Colors Nord_colors[] = {
    NORD0, NORD1, NORD2,  NORD3,  NORD4,  NORD5,  NORD6,  NORD7,
    NORD8, NORD9, NORD10, NORD11, NORD12, NORD13, NORD14, NORD15};

uint32_t get_Nord_Color_as_uint32(Nord_Colors color) { return (uint32_t)color; }
Nord_Colors get_Nord_Color_by_index(int index) { return Nord_colors[index]; }

uint32_t get_Nord_Color_as_uint32_by_index(int index) {
  return get_Nord_Color_as_uint32(get_Nord_Color_by_index(index));
}

#ifdef RAYLIB_H

Color get_Nord_Color_as_raylib_color(Nord_Colors color) {
  return GetColor(get_Nord_Color_as_uint32(color));
}

Color get_Nord_Color_as_raylib_color_by_index(int index) {
  return get_Nord_Color_as_raylib_color(get_Nord_Color_by_index(index));
}

#endif

// this is a personel addition section for better theming and color usage, this
// is not part of the original nord color palette

#ifdef RAYLIB_H

#define NORD_BACKGROUND_COLOR get_Nord_Color_as_raylib_color(NORD0)
#define NORD_FOREGROUND_COLOR get_Nord_Color_as_raylib_color(NORD6)
#define NORD_TEXT_COLOR NORD_FOREGROUND_COLOR
#define NORD_PRIMARY_COLOR get_Nord_Color_as_raylib_color(NORD14)
#define NORD_ACCENT_COLOR get_Nord_Color_as_raylib_color(NORD11)
#define NORD_HIGHLIGHT_COLOR NORD_ACCENT_COLOR
#define NORD_ERROR_COLOR get_Nord_Color_as_raylib_color(NORD11)
#define NORD_WARNING_COLOR get_Nord_Color_as_raylib_color(NORD12)
#define NORD_INFO_COLOR get_Nord_Color_as_raylib_color(NORD6)
#define NORD_TRACE_COLOR get_Nord_Color_as_raylib_color(NORD13)
#define NORD_DEBUG_COLOR get_Nord_Color_as_raylib_color(NORD15)
#define NORD_FATAL_COLOR get_Nord_Color_as_raylib_color(NORD11)
#define NORD_SUCCESS_COLOR get_Nord_Color_as_raylib_color(NORD14)

#else

#define NORD_BACKGROUND_COLOR get_Nord_Color_as_uint32(NORD0)
#define NORD_FOREGROUND_COLOR get_Nord_Color_as_uint32(NORD6)
#define NORD_TEXT_COLOR NORD_FOREGROUND_COLOR
#define NORD_PRIMARY_COLOR get_Nord_Color_as_uint32(NORD14)
#define NORD_ACCENT_COLOR get_Nord_Color_as_uint32(NORD11)
#define NORD_HIGHLIGHT_COLOR NORD_ACCENT_COLOR
#define NORD_ERROR_COLOR get_Nord_Color_as_uint32(NORD11)
#define NORD_WARNING_COLOR get_Nord_Color_as_uint32(NORD12)
#define NORD_INFO_COLOR get_Nord_Color_as_uint32(NORD6)
#define NORD_TRACE_COLOR get_Nord_Color_as_uint32(NORD13)
#define NORD_DEBUG_COLOR get_Nord_Color_as_uint32(NORD15)
#define NORD_FATAL_COLOR get_Nord_Color_as_uint32(NORD11)
#define NORD_SUCCESS_COLOR get_Nord_Color_as_uint32(NORD14)

#endif

#endif // NORD_COLORS_H
