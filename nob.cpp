#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#include "nob_addons.h"
#define FLAG_IMPLEMENTATION
#include "flag.h"

#define BUILD_FOLDER "build/"
#define SOURCE_FOLDER "src/"
#define TOOLS_SRC_FOLDER "tools/"
#define TOOLS_BUILD_FOLDER BUILD_FOLDER "tools/"
#define THIRDPARTY_FOLDER "thirdparty/"
#define THIRDPARTY_INCLUDE_FOLDER THIRDPARTY_FOLDER "include/"

#define EXECUTABLE BUILD_FOLDER "chess-engine-battel"

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

bool build_tools(bool debug) {
  if (!mkdir_if_not_exists(TOOLS_BUILD_FOLDER)) { return 1; }
  clangpp(&cmd);
  cmd_append(&cmd, TOOLS_SRC_FOLDER "assets2c.cpp");
  clangpp_flags(&cmd);
  if (debug) {
    cmd_append(&cmd, "-g");
  } else {
    cmd_append(&cmd, "-O3");
  }
  clangpp_flags(&cmd);
  cmd_append(&cmd, "-I", THIRDPARTY_INCLUDE_FOLDER);
  cmd_append(&cmd, "-I", TOOLS_SRC_FOLDER);
  cmd_append(&cmd, "-I", SOURCE_FOLDER);
  cmd_append(&cmd, "-I", ".");
  cmd_append(&cmd, "-L", BUILD_FOLDER "raylib");
  cmd_append(&cmd, "-lraylib");
  cmd_append(&cmd, "-lm");
#ifdef __APPLE__

  cmd_append(&cmd, "-mmacos-version-min=26.0");

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
  cmd_append(&cmd, "-lWayland-client");
#elif defined(_WIN32)
  cmd_append(&cmd, "-lwinmm");
// more platforms can be added here
#endif

  cmd_append(&cmd, "-Wno-unused-function");
  cmd_append(&cmd, "-Wno-unused-variable");
  cmd_append(&cmd, "-Wno-unused-parameter");
  cmd_append(&cmd, "-Wno-missing-field-initializers");
  cmd_append(&cmd, "-Wno-unused-value");
  cmd_append(&cmd, "-Wno-writable-strings");
  cmd_append(&cmd, "-Wno-unused-command-line-argument");

  cmd_append(&cmd, "-static-libclosure", "-static-libsan", "-static-openmp");

  cmd_append(&cmd, "-o", TOOLS_BUILD_FOLDER "assets2c");
  return cmd_run(&cmd);
}

bool run_tools(bool debug) {
  if (!build_tools(debug)) { return 1; }
  cmd_append(&cmd, TOOLS_BUILD_FOLDER "assets2c");
  return cmd_run(&cmd);
}

bool build_raylib(bool debug) {
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
  cmd_append(&cmd, "EXTERNAL_CONFIG_FLAGS=-DSUPPORT_FILEFORMAT_FLAC=1");
  if (debug) {
    cmd_append(&cmd,
               "EXTERNAL_CONFIG_FLAGS+=-DRLGL_ENABLE_OPENGL_DEBUG_CONTEXT=1");
  }
  cmd_append(&cmd, "EXTERNAL_CONFIG_FLAGS+=-DRLGL_SHOW_GL_DETAILS_INFO=1");
#ifdef __APPLE__
  cmd_append(&cmd, "EXTERNAL_CONFIG_FLAGS+=-mmacos-version-min=26.0");
#endif
  return cmd_run(&cmd);
}

int main(int argc, char **argv) {
  addon_init_logging();
  F_Args f_args = {0};

  for (int i = 0; i < argc; ++i) { da_append(&f_args, strdup(argv[i])); }
  NOB_GO_REBUILD_URSELF(argc, argv);

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

    if (!run_tools(debug)) { return 1; }

    clangpp(&cmd);
    cmd_append(&cmd, SOURCE_FOLDER "main.cpp");
    if (debug) {
      cmd_append(&cmd, "-g");
    } else {
      cmd_append(&cmd, "-O3");
    }
    clangpp_flags(&cmd);
    cmd_append(&cmd, "-I", THIRDPARTY_INCLUDE_FOLDER);
    cmd_append(&cmd, "-I", TOOLS_SRC_FOLDER);
    cmd_append(&cmd, "-I", SOURCE_FOLDER);
    cmd_append(&cmd, "-I", BUILD_FOLDER "assets");
    cmd_append(&cmd, "-I", ".");
    cmd_append(&cmd, "-L", BUILD_FOLDER "raylib");
    cmd_append(&cmd, "-lraylib");
    cmd_append(&cmd, "-lm");
#ifdef __APPLE__

    cmd_append(&cmd, "-mmacos-version-min=26.0");

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
// more platforms can be added here
#endif

    cmd_append(&cmd, "-Wno-unused-function");
    cmd_append(&cmd, "-Wno-unused-variable");
    cmd_append(&cmd, "-Wno-unused-parameter");
    cmd_append(&cmd, "-Wno-missing-field-initializers");
    cmd_append(&cmd, "-Wno-unused-value");
    cmd_append(&cmd, "-Wno-writable-strings");
    cmd_append(&cmd, "-Wno-unused-command-line-argument");

    cmd_append(&cmd, "-static-libclosure", "-static-libsan", "-static-openmp");

    cmd_append(&cmd, "-o", EXECUTABLE);
    if (!cmd_run(&cmd)) { return 1; }
  }

  if (debug) { cmd_append(&cmd, debugger); }

  if (run) {
    cmd_append(&cmd, EXECUTABLE);
    if (!cmd_run(&cmd)) { return 1; }
  }

#if defined(__APPLE__)
  if (compile && !run && !debug) {
    // Pfade sauber als Variablen definieren
    const char *app_dir = BUILD_FOLDER "chess-engine-battel.app";
    const char *contents_dir = BUILD_FOLDER "chess-engine-battel.app/Contents";
    const char *macos_dir =
        BUILD_FOLDER "chess-engine-battel.app/Contents/MacOS";
    const char *plist_path =
        BUILD_FOLDER "chess-engine-battel.app/Contents/Info.plist";
    const char *pkginfo_path =
        BUILD_FOLDER "chess-engine-battel.app/Contents/PkgInfo";

    cmd_append(&cmd, "rm", "-rf", app_dir);

    if (!cmd_run(&cmd)) { return 1; }

    // 1. Verzeichnisstruktur erstellen
    if (!mkdir_if_not_exists(app_dir)) { return 1; }
    if (!mkdir_if_not_exists(contents_dir)) { return 1; }
    if (!mkdir_if_not_exists(macos_dir)) { return 1; }

    // 4. Dein Icon-Verzeichnis kopieren
    if (!copy_directory_recursively(
            "assets-macos-extra",
            BUILD_FOLDER "chess-engine-battel.app/Contents/Resources")) {
      return 1;
    }

    // 2. Executable in das MacOS-Verzeichnis kopieren
    if (!copy_file(
            EXECUTABLE, BUILD_FOLDER
            "chess-engine-battel.app/Contents/MacOS/chess-engine-battel")) {
      return 1;
    }

    // 3. Info.plist schreiben (Fehler aus dem Originalcode behoben)
    //        "   <key>DTCompiler</key>\n"
    //        "      <string>com.apple.compilers.llvm.clang.1_0 </string>\n"
    const char *plist_content =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
        "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
        "<plist version=\"1.0\">\n"
        "<dict>\n"
        "   <key>BuildMachineOSBuild</key>\n"
        "       <string>26A5416b</string>\n"
        "   <key>CFBundleDevelopmentRegion</key>\n"
        "       <string>en</string>\n"
        "   <key>CFBundleDisplayName</key>\n"
        "       <string>comboom.sucht Chess Battle</string>\n"
        "   <key>CFBundleExecutable</key>\n"
        "       <string>chess-engine-battel</string>\n"
        "   <key>CFBundleIconFile</key>\n"
        "       <string>AppIcon</string>\n"
        "   <key>CFBundleIconName</key>\n"
        "       <string>AppIcon</string>\n"
        "   <key>CFBundleIdentifier</key>\n"
        "      <string>dev.comboom.sucht.chess-engine-battel</string>\n"
        "   <key>CFBundleInfoDictionaryVersion</key>\n"
        "      <string>6.0</string>\n"
        "   <key>CFBundleName</key>\n"
        "      <string>comboom.sucht Chess Battle</string>\n"
        "   <key>CFBundlePackageType</key>\n"
        "      <string>APPL</string>\n"
        "   <key>CFBundleShortVersionString</key>\n"
        "      <string>0.0.1</string>\n"
        "   <key>CFBundleSupportedPlatforms</key>\n"
        "      <array>\n"
        "         <string>MacOSX</string>\n"
        "      </array>\n"
        "   <key>CFBundleVersion</key>\n"
        "      <string>0.0.1</string>\n"
        "   <key>DTPlatformBuild</key>\n"
        "      <string>26A5406c</string>\n"
        "   <key>DTPlatformName</key>\n"
        "      <string>macosx</string>\n"
        "   <key>DTPlatformVersion</key>\n"
        "      <string>27.0</string>\n"
        "   <key>DTSDKBuild</key>\n"
        "      <string>26A5406c</string>\n"
        "   <key>DTSDKName</key>\n"
        "      <string>macosx27.0</string>\n"
        "   <key>DTXcode</key>\n"
        "      <string>2700</string>\n"
        "   <key>DTXcodeBuild</key>\n"
        "      <string>27A5237l</string>\n"
        "   <key>LSApplicationCategoryType</key>\n"
        "      <string>public.app-category.games</string>\n"
        "   <key>LSMinimumSystemVersion</key>\n"
        "      <string>26.0</string>\n"
        "   <key>LSUIElement</key>\n"
        "      <true/>\n"
        "   <key>NSHighResolutionCapable</key>\n"
        "      <true/>\n"
        "   <key>NSHumanReadableCopyright</key>\n"
        "      <string>Copyright © 2026 comboom.sucht.Alle Rechte "
        "vorbehalten.</string>\n"
        "   </dict>\n"
        "   </plist>";

    const char *pkginfo_content = "\x41\x50\x50\x4C\x3f\x3f\x3f\x3f";

    if (!write_entire_file(plist_path, plist_content, strlen(plist_content))) {
      nob_log(ERROR, "Konnte Info.plist nicht erstellen: %s", plist_path);
      return 1;
    };

    if (!write_entire_file(pkginfo_path, pkginfo_content,
                           strlen(pkginfo_content))) {
      nob_log(ERROR, "Konnte PkgInfo nicht erstellen: %s", pkginfo_path);
      return 1;
    };
  }
#endif

  return 0;
}
