#include <stdio.h>
#include <stdlib.h>
#define NOB_IMPLEMENTATION
#include "nob.h"
#include "nob_addons.h"
#define FLAG_IMPLEMENTATION
#include "flag.h"

#define BUILD_FOLDER "build/"
#define SOURCE_FOLDER "src/"
#define THIRDPARTY_FOLDER "thirdparty/"

#define EXECUTABLE (BUILD_FOLDER "chess-engine")

Cmd cmd = {0};

typedef struct {
    char **items;
    size_t count;
    size_t capacity;

} F_Args;

void usage(FILE *stream) {
  fprintf(stream, "Usage: ./nob [OPTIONS]\n");
  fprintf(stream, "OPTIONS:\n");
  flag_print_options(stream);
}

void print_list(const char **items, size_t count) {
  printf("[");
  for (size_t i = 0; i < count; ++i) {
    if (i > 0) { printf(", "); }
    printf("%s", items[i]);
  }
  printf("]\n");
}

bool build_raylib(bool debug) {

  if (!mkdir_if_not_exists(BUILD_FOLDER)) { return 1; }
  if (!mkdir_if_not_exists(BUILD_FOLDER "raylib")) { return 1; }

  cmd_append(&cmd, "make", "-C", THIRDPARTY_FOLDER "raylib/src");
  cmd_append(&cmd, "PLATFORM=PLATFORM_DESKTOP");
  cmd_append(&cmd, "RAYLIB_LIBTYPE=STATIC");
  if (debug) {
    cmd_append(&cmd, "RAYLIB_BUILD_MODE=DEBUG");
  } else {
    cmd_append(&cmd, "RAYLIB_BUILD_MODE=RELEASE");
  }
  cmd_append(&cmd, "RAYLIB_MODULE_RAYGUI=TRUE");
  cmd_append(&cmd, "RAYLIB_MODULE_RAYGUI_PATH=../../raygui/src");
  cmd_append(&cmd, "RAYLIB_RELEASE_PATH=../../../build/raylib");
  return cmd_run(&cmd);
}

int main(int argc, char **argv) {
  F_Args f_args = {0};

  for (int i = 0; i < argc; ++i) { da_append(&f_args, strdup(argv[i])); }
  NOB_GO_REBUILD_URSELF(argc, argv);

  nob_set_log_handler(nob_cancer_log_handler);

  bool  help = false;
  bool  compile = false;
  bool  debug = false;
  char *debugger = (char *)"lldb";
  bool  run = false;
  flag_bool_var(&help, "-help", false,
                "Print this help to stdout and exit with 0");
  flag_bool_var(&help, "h", false, "Print this help to stdout and exit with 0");
  flag_bool_var(&compile, "-compile", false, "Compile the project");
  flag_bool_var(&compile, "c", false, "Compile the project");

  flag_bool_var(&debug, "-debug", false, "Build in debug mode");
  flag_bool_var(&debug, "d", false, "Build in debug mode");

  flag_bool_var(&run, "-run", false, "Run the project");
  flag_bool_var(&run, "r", false, "Run the project");

  flag_str_var(&debugger, "-debugger", "lldb", "The debugger to use");

  if (f_args.count == 1) {
    usage(stderr);
    exit(1);
  }

  if (!flag_parse(f_args.count, f_args.items)) {
    usage(stderr);
    flag_print_error(stderr);
    exit(1);
  }

  f_args.count = flag_rest_argc();
  f_args.items = flag_rest_argv();

  if (help) {
    usage(stdout);
    exit(0);
  }

  if (compile) {
    if (!mkdir_if_not_exists(BUILD_FOLDER)) { return 1; }
    if (!build_raylib(debug)) { return 1; }
    clangpp(&cmd);
    cmd_append(&cmd, SOURCE_FOLDER "main.cpp");
    if (debug) {
      cmd_append(&cmd, "-g");
    } else {
      cmd_append(&cmd, "-O3");
    }
    clangpp_flags(&cmd);
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "nob.h");
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "flag.h");
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "ht.h");
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "ht.h");
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "raylib/src");
    cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "raygui/src");
    cmd_append(&cmd, "-I", ".");
    cmd_append(&cmd, "-L", BUILD_FOLDER "raylib");
    cmd_append(&cmd, "-lraylib");
    cmd_append(&cmd, "-lm");
#ifdef __APPLE__
    cmd_append(&cmd, "-framework", "OpenGL");
    cmd_append(&cmd, "-framework", "Cocoa");
    cmd_append(&cmd, "-framework", "IOKit");
    cmd_append(&cmd, "-framework", "CoreVideo");
    cmd_append(&cmd, "-framework", "QuartzCore");
#elif defined(__linux__)
    cmd_append(&cmd, "-ldl");
    cmd_append(&cmd, "-lrt");
    cmd_append(&cmd, "-lpthread");
    cmd_append(&cmd, "-lX11");
    cmd_append(&cmd, "-lGL");
    cmd_append(&cmd, "-lGLU");
#elif defined(_WIN32)
    cmd_append(&cmd, "-lopengl32");
    cmd_append(&cmd, "-lgdi32");
    cmd_append(&cmd, "-lwinmm");
#endif

    cmd_append(&cmd, "-Wno-unused-function");
    cmd_append(&cmd, "-Wno-unused-variable");
    cmd_append(&cmd, "-Wno-unused-parameter");
    cmd_append(&cmd, "-Wno-missing-field-initializers");
    cmd_append(&cmd, "-Wno-unused-value");
    cmd_append(&cmd, "-Wno-writable-strings");

    cmd_append(&cmd, "-o", EXECUTABLE);
    if (!cmd_run(&cmd)) { return 1; }
  }

  if (debug) { cmd_append(&cmd, debugger); }

  if (run) {
    cmd_append(&cmd, EXECUTABLE);
    if (!cmd_run(&cmd)) { return 1; }
  }
  return 0;
}
