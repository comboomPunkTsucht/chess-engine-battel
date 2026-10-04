#include "nob.h"
#include <stdint.h>

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

#define NORD_ANSI_RGB(color)                                                   \
  (((color) >> 24) & 0xFF), (((color) >> 16) & 0xFF), (((color) >> 8) & 0xFF)

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

#ifdef NOB_H_

// 1. Eigene, erweiterte Loglevel definieren
#define NOB_TRACE ((Nob_Log_Level)3)
#define NOB_DEBUG ((Nob_Log_Level)4)
#define NOB_FATAL ((Nob_Log_Level)5)

// 2. Deklarationen
NOBDEF void nob_addon_advanced_log_handler(Nob_Log_Level level, const char *fmt,
                                           va_list args);
NOBDEF void
    nob_addon_init_logging(void); // <-- Deine neue All-in-One Setup-Funktion

NOBDEF void nob_breakpoint(void);

#ifdef RAYLIB_H
#include <stdarg.h>
#include <stdio.h>
NOBDEF void nob_addon_RaylibLog(int msgType, const char *text, va_list args);
#endif

#ifdef NOB_IMPLEMENTATION
#define nob_clangpp(cmd) nob_cmd_append(cmd, "clang++")
#define nob_clangpp_flags(cmd)                                                 \
  nob_cmd_append(cmd, "-Wall", "-Wextra", "-std=c++26")

/* Hilfsmakro, falls der Compiler __has_builtin nicht unterstützt */
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

NOBDEF void nob_breakpoint(void) {
#if defined(_MSC_VER)
  /* Microsoft Visual Studio Compiler */
  __debugbreak();
#elif __has_builtin(__builtin_debugtrap)
  /* Clang bietet ein eigenes Built-in für saubere Breakpoints */
  __builtin_debugtrap();
#elif defined(__x86_64__) || defined(__i386__)
  /* x86 / x86_64 Architektur */
  __asm__ volatile("int3");
#elif defined(__aarch64__)
  /* ARM 64-Bit */
  __asm__ volatile("brk #0");
#elif defined(__arm__)
  /* ARM 32-Bit */
  __asm__ volatile("bkpt #0");
#elif defined(__riscv)
  /* RISC-V Architektur */
  __asm__ volatile("ebreak");
#elif defined(__powerpc__)
  /* PowerPC Architektur */
  __asm__ volatile("trap");
#elif defined(__GNUC__) || defined(__clang__)
  /* Generischer GCC-Fallback (Achtung: Beendet das Programm oft komplett) */
  __builtin_trap();
#else
  /* Letzter Ausweg für unbekannte Compiler: Absichtlicher Segfault */
  *(volatile int *)0 = 0;
#endif
}

NOBDEF void nob_addon_advanced_log_handler(Nob_Log_Level level, const char *fmt,
                                           va_list args) {
  switch ((int)level) {
    case 0:
      fprintf(stderr, "ℹ️  \x1b[38;2;%d;%d;%dm[INFO]\x1b[0m ",
              NORD_ANSI_RGB(NORD6));
      break; // NOB_INFO
    case 1:
      fprintf(stderr, "⚠️  \x1b[38;2;%d;%d;%dm[WARN]\x1b[0m ",
              NORD_ANSI_RGB(NORD12));
      break; // NOB_WARNING
    case 2:
      fprintf(stderr, "🚨  \x1b[38;2;%d;%d;%dm[ERROR]\x1b[0m ",
              NORD_ANSI_RGB(NORD11));
      break; // NOB_ERROR
    case 3:
      fprintf(stderr, "🔍  \x1b[38;2;%d;%d;%dm[TRACE]\x1b[0m ",
              NORD_ANSI_RGB(NORD13));
      break; // NOB_TRACE
    case 4:
      fprintf(stderr, "🐛  \x1b[38;2;%d;%d;%dm[DEBUG]\x1b[0m ",
              NORD_ANSI_RGB(NORD15));
      break; // NOB_DEBUG
    case 5:
      fprintf(stderr, "💀  \x1b[38;2;%d;%d;%dm[FATAL]\x1b[0m ",
              NORD_ANSI_RGB(NORD11));
      break; // NOB_FATAL
    default: fprintf(stderr, "   [LOG] "); break;
  }
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
}

#ifdef RAYLIB_H

NOBDEF void nob_addon_RaylibLog(int msgType, const char *text, va_list args) {
  char buffer[1024] = {0};
  vsnprintf(buffer, sizeof(buffer), text, args);

  switch (msgType) {
    case LOG_TRACE  : nob_log(NOB_TRACE, "[raylib] %s", buffer); break;
    case LOG_DEBUG  : nob_log(NOB_DEBUG, "[raylib] %s", buffer); break;
    case LOG_INFO   : nob_log((Nob_Log_Level)0, "[raylib] %s", buffer); break;
    case LOG_WARNING: nob_log((Nob_Log_Level)1, "[raylib] %s", buffer); break;
    case LOG_ERROR  : nob_log((Nob_Log_Level)2, "[raylib] %s", buffer); break;
    case LOG_FATAL  : nob_log(NOB_FATAL, "[raylib] %s", buffer); break;
    default         : break;
  }
}
#endif // RAYLIB_H

NOBDEF void nob_addon_init_logging(void) {
  // Nob-Logging immer setzen
  nob_set_log_handler(nob_addon_advanced_log_handler);

#ifdef RAYLIB_H
  // Raylib-Logging nur einklinken, wenn raylib.h vorher eingebunden wurde
  SetTraceLogCallback(nob_addon_RaylibLog);
#endif
}
#endif

#ifndef NOB_UNSTRIP_PREFIX
#define clangpp nob_clangpp
#define clangpp_flags nob_clangpp_flags
#define breakpoint nob_breakpoint
#define addon_advanced_log_handler nob_addon_advanced_log_handler
#define addon_init_logging nob_addon_init_logging
#ifdef RAYLIB_H
#define addon_RaylibLog nob_addon_RaylibLog
#endif
#endif

#endif
