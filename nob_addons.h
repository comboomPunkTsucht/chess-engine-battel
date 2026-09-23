#ifdef NOB_H_

NOBDEF void nob_breakpoint(void);

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
#endif

#ifndef NOB_UNSTRIP_PREFIX
#define clangpp nob_clangpp
#define clangpp_flags nob_clangpp_flags
#define breakpoint nob_breakpoint
#endif

#endif
