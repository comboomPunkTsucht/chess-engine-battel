#include "nob.h"

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
    case 0 : fprintf(stderr, "ℹ️  \x1b[36m[INFO]\x1b[0m "); break; // NOB_INFO
    case 1 : fprintf(stderr, "⚠️  \x1b[33m[WARN]\x1b[0m "); break; // NOB_WARNING
    case 2 : fprintf(stderr, "🚨  \x1b[31m[ERROR]\x1b[0m "); break; // NOB_ERROR
    case 3 : fprintf(stderr, "🔍  \x1b[35m[TRACE]\x1b[0m "); break; // NOB_TRACE
    case 4 : fprintf(stderr, "🐛  \x1b[32m[DEBUG]\x1b[0m "); break; // NOB_DEBUG
    case 5 : fprintf(stderr, "💀  \x1b[31m[FATAL]\x1b[0m "); break; // NOB_FATAL
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
