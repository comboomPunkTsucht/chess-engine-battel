#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#include "nob_addons.h"
#define FLAG_IMPLEMENTATION
#include "flag.h"

#define VERSION "0.0.2"

#define BUILD_FOLDER "build/"
#define SOURCE_FOLDER "src/"
#define TOOLS_SRC_FOLDER "tools/"
#define TOOLS_BUILD_FOLDER BUILD_FOLDER "tools/"
#define THIRDPARTY_FOLDER "thirdparty/"
#define THIRDPARTY_INCLUDE_FOLDER THIRDPARTY_FOLDER "include/"
#ifdef _WIN32
#define BUILD_EXECUTABLE BUILD_FOLDER "chess-engine-battel.exe"
#else
#define BUILD_EXECUTABLE BUILD_FOLDER "chess-engine-battel"
#endif
#ifdef __APPLE__
#define EXECUTABLE "./chess-engine-battel"
#else
#define EXECUTABLE BUILD_EXECUTABLE
#endif

Cmd cmd = {0};

const char *cwd = get_current_dir_temp();

#ifdef __linux__
bool with_x11 = false;
#endif

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
	if (!mkdir_if_not_exists(BUILD_FOLDER "raylib")) { return 1; }

	bool raylib_debug = debug;
#ifdef _WIN32
	raylib_debug = false;
#endif
	const char *build_type = raylib_debug ? "Debug" : "Release";

	const char *c_flags = "-DSUPPORT_FILEFORMAT_FLAC=1 -DRLGL_SHOW_GL_DETAILS_INFO=1";
	if (raylib_debug) { c_flags = temp_sprintf("%s -DRLGL_ENABLE_OPENGL_DEBUG_CONTEXT=1", c_flags); }
#ifdef _WIN32
	c_flags = temp_sprintf("/DWIN32 /D_WINDOWS /W3 %s", c_flags);
#endif

	cmd_append(&cmd, "cmake");
	cmd_append(&cmd, "-DBUILD_EXAMPLES=OFF");
	cmd_append(&cmd, "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded");
	cmd_append(&cmd, temp_sprintf("-DCMAKE_BUILD_TYPE=%s", build_type));
	cmd_append(&cmd, temp_sprintf("-DCMAKE_C_FLAGS=%s", c_flags));
#ifdef __APPLE__
	cmd_append(&cmd, "-DCMAKE_OSX_DEPLOYMENT_TARGET=26.0");
#endif
#ifdef __linux__
	cmd_append(&cmd, "-DGLFW_BUILD_WAYLAND=ON");
	if (with_x11) {
		cmd_append(&cmd, "-DGLFW_BUILD_X11=ON");
	} else {
		cmd_append(&cmd, "-DGLFW_BUILD_X11=OFF");
	}
#endif
	cmd_append(&cmd, temp_sprintf("-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=%s/%s", cwd, BUILD_FOLDER "raylib"));
	cmd_append(&cmd, "-B", BUILD_FOLDER "raylib");
	cmd_append(&cmd, "-S", THIRDPARTY_FOLDER "raylib");
	if (!cmd_run(&cmd)) { return false; }

	cmd_append(&cmd, "cmake");
	cmd_append(&cmd, "--build", BUILD_FOLDER "raylib");
	cmd_append(&cmd, "--config", build_type);
	return cmd_run(&cmd);
}

bool build_ixwebsocket(bool debug) {
	if (!mkdir_if_not_exists(BUILD_FOLDER "ixwebsocket")) { return false; }
	cmd_append(&cmd, "cmake");
	cmd_append(&cmd, "-DUSE_TLS=1");
	cmd_append(&cmd, "-DUSE_OPEN_SSL=1");
#ifdef _WIN32
	cmd_append(&cmd, "-DUSE_ZLIB=0");
	cmd_append(&cmd, "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded");
	cmd_append(&cmd, "-DCMAKE_CXX_FLAGS_RELEASE=/MT /O2 /Ob2 /DNDEBUG");
	cmd_append(&cmd, "-DCMAKE_C_FLAGS_RELEASE=/MT /O2 /Ob2 /DNDEBUG");
#else
	cmd_append(&cmd, "-DUSE_ZLIB=1");
#endif
	cmd_append(&cmd, "--install-prefix", temp_sprintf("%s%s", cwd, BUILD_FOLDER "ixwebsocket"));
	cmd_append(&cmd, "-B", BUILD_FOLDER "ixwebsocket");
	cmd_append(&cmd, "-S", THIRDPARTY_FOLDER "IXWebSocket");

	if (!cmd_run(&cmd)) { return false; }

	cmd_append(&cmd, "cmake");
	cmd_append(&cmd, "--build", BUILD_FOLDER "ixwebsocket");
	cmd_append(&cmd, "--config", "Release");
	if (!cmd_run(&cmd)) { return false; }

	return true;
}

bool get_cflags_from_pkg_config(const char *package) {
	Cmd temp = {0};
	cmd_append(&temp, "pkg-config");
	cmd_append(&temp, "--cflags");
	cmd_append(&temp, package);
	if (!cmd_run(&temp, .stdout_path = BUILD_FOLDER "cflags.temp.txt")) { return false; }

	String_Builder cflags = {0};
	if (!read_entire_file(BUILD_FOLDER "cflags.temp.txt", &cflags)) { return false; }

	String_Builder flag = {0};
	for (size_t i = 0; i < cflags.count; ++i) {
		char c = cflags.items[i];
		if (c == ' ' || c == '\n') {
			if (flag.count > 0) {
				da_append(&flag, '\0');
				// FIX: Den String für cmd duplizieren, damit er nicht überschrieben wird
				cmd_append(&cmd, temp_strdup(flag.items));
				flag.count = 0;
			}
		} else {
			da_append(&flag, c);
		}
	}
	if (flag.count > 0) {
		da_append(&flag, '\0');
		cmd_append(&cmd, temp_strdup(flag.items));
	}

	da_free(flag);
	da_free(cflags);
	return true;
}

bool get_libs_from_pkg_config(const char *package) {
	Cmd temp = {0};
	cmd_append(&temp, "pkg-config");
	cmd_append(&temp, "--libs");
	cmd_append(&temp, package);
	if (!cmd_run(&temp, .stdout_path = BUILD_FOLDER "libs.temp.txt")) { return false; }

	String_Builder libs = {0};
	if (!read_entire_file(BUILD_FOLDER "libs.temp.txt", &libs)) { return false; }

	String_Builder flag = {0};
	for (size_t i = 0; i < libs.count; ++i) {
		char c = libs.items[i];
		if (c == ' ' || c == '\n') {
			if (flag.count > 0) {
				da_append(&flag, '\0');
				// FIX: Den String für cmd duplizieren
				cmd_append(&cmd, temp_strdup(flag.items));
				flag.count = 0;
			}
		} else {
			da_append(&flag, c);
		}
	}
	if (flag.count > 0) {
		da_append(&flag, '\0');
		cmd_append(&cmd, temp_strdup(flag.items));
	}

	da_free(flag);
	da_free(libs);
	return true;
}

int main(int argc, char **argv) {
	addon_init_logging();
	F_Args f_args = {0};

	for (int i = 0; i < argc; ++i) { da_append(&f_args, strdup(argv[i])); }
	NOB_GO_REBUILD_URSELF(argc, argv);

	bool help = false;
	bool compile = false;
	bool release = false;
	bool debug = false;
	char *debugger = (char *)"lldb";
	bool run = false;

	char *install = NULL;

	flag_bool_var(&help, "-help", false, "Print this help to stdout and exit with 0");
	flag_bool_var(&help, "h", false, "Print this help to stdout and exit with 0");
	flag_bool_var(&compile, "-compile", false, "Compile the project");
	flag_bool_var(&compile, "c", false, "Compile the project");

	flag_bool_var(&debug, "-debug", false, "Build in debug mode");
	flag_bool_var(&debug, "d", false, "Build in debug mode");

	flag_bool_var(&run, "-run", false, "Run the project");
	flag_bool_var(&run, "r", false, "Run the project");

	flag_str_var(&debugger, "-debugger", "lldb", "The debugger to use");
	flag_bool_var(&release, "-release", false, "Build in release mode");

	flag_str_var(&install, "-install", NULL, "Install the project at the specified path");

#ifdef __linux__
	flag_bool_var(&with_x11, "-with-x11", false, "Build with X11 support");
#endif

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
		if (!build_ixwebsocket(debug)) { return 1; }
		if (!build_raylib(debug)) { return 1; }

		clangpp(&cmd);
		cmd_append(&cmd, SOURCE_FOLDER "main.cpp");
		cmd_append(&cmd, SOURCE_FOLDER "app.cpp");
		cmd_append(&cmd, SOURCE_FOLDER "game.cpp");
		cmd_append(&cmd, SOURCE_FOLDER "update.cpp");
		cmd_append(&cmd, SOURCE_FOLDER "processInput.cpp");
		cmd_append(&cmd, SOURCE_FOLDER "render.cpp");
		cmd_append(&cmd, SOURCE_FOLDER "wsConnection.cpp");

		if (debug) {
			cmd_append(&cmd, "-g");
			// NEU: Zwingt Clang, die .o Datei zu behalten, damit dsymutil sie
			// lesen kann
			cmd_append(&cmd, "-save-temps=obj");
			// NEU: Legt die .o Datei sauber im "build/"-Ordner ab anstatt im
			// Hauptverzeichnis
			cmd_append(&cmd, "-dumpdir", BUILD_FOLDER);
			if (!release) {
				cmd_append(&cmd, "-O0");
			} else {
				cmd_append(&cmd, "-O3");
			}

		} else if (!release) {
			cmd_append(&cmd, "-O0");
		} else {
			cmd_append(&cmd, "-O3");
		}

		clangpp_flags(&cmd);
		// FIX: Windows kann dem Ordner-Symlink thirdparty/include/ixwebsocket
		// nicht folgen, daher direkt auf das Submodul zeigen.
		cmd_append(&cmd, "-I", THIRDPARTY_FOLDER "IXWebSocket");
		cmd_append(&cmd, "-I", THIRDPARTY_INCLUDE_FOLDER);
		cmd_append(&cmd, "-I", TOOLS_SRC_FOLDER);
		cmd_append(&cmd, "-I", SOURCE_FOLDER);
		cmd_append(&cmd, "-I", BUILD_FOLDER "assets");
		cmd_append(&cmd, "-I", ".");

#ifdef _WIN32
		cmd_append(&cmd, "-static");
		cmd_append(&cmd, "-static-libgcc", "-static-libstdc++");
		cmd_append(&cmd, "-fuse-ld=lld");
		cmd_append(&cmd, BUILD_FOLDER "raylib/Release/raylib.lib");
		cmd_append(&cmd, BUILD_FOLDER "ixwebsocket/Release/ixwebsocket.lib");
		cmd_append(&cmd, "C:/Program Files/OpenSSL/lib/VC/x64/MD/libssl.lib");
		cmd_append(&cmd, "C:/Program Files/OpenSSL/lib/VC/x64/MD/libcrypto.lib");
#else
		// Da wir raylib und ixwebsocket selbst lokal bauen,
		// müssen wir sie manuell verlinken.
		cmd_append(&cmd, "-L", BUILD_FOLDER "ixwebsocket");
		cmd_append(&cmd, "-L", BUILD_FOLDER "raylib");
		cmd_append(&cmd, "-lraylib");
		cmd_append(&cmd, "-lixwebsocket");
		cmd_append(&cmd, "-lm");
		cmd_append(&cmd, "-lz");
#endif

#ifdef __APPLE__
		if (!get_cflags_from_pkg_config("openssl")) { return 1; }
		if (!get_libs_from_pkg_config("openssl")) { return 1; }

		cmd_append(&cmd, "-mmacos-version-min=26.0");
		cmd_append(&cmd, "-static-libclosure", "-static-libsan", "-static-openmp");

		// macOS Frameworks besitzen keine pkg-config Dateien
		cmd_append(&cmd, "-framework", "Security");
		cmd_append(&cmd, "-framework", "Foundation");
		cmd_append(&cmd, "-framework", "OpenGL");
		cmd_append(&cmd, "-framework", "Cocoa");
		cmd_append(&cmd, "-framework", "IOKit");
		cmd_append(&cmd, "-framework", "CoreVideo");
		cmd_append(&cmd, "-framework", "QuartzCore");

#elif defined(__linux__)
		cmd_append(&cmd, "-static-libgcc", "-static-libstdc++");
		if (with_x11) {
			// Zieht automatisch -lX11 und ggf. nötige Includes
			if (!get_cflags_from_pkg_config("x11")) { return 1; }
			if (!get_libs_from_pkg_config("x11")) { return 1; }
			cmd_append(&cmd, "-DUSE_X11");
		}

		if (!get_cflags_from_pkg_config("openssl")) { return 1; }
		if (!get_libs_from_pkg_config("openssl")) { return 1; }
		// Ersetzt manuelles -lGL und -lGLU
		if (!get_cflags_from_pkg_config("gl")) { return 1; }
		if (!get_libs_from_pkg_config("gl")) { return 1; }
		if (!get_cflags_from_pkg_config("glu")) { return 1; }
		if (!get_libs_from_pkg_config("glu")) { return 1; }

		// Standard POSIX/C-Libraries bleiben hardcoded
		cmd_append(&cmd, "-ldl");
		cmd_append(&cmd, "-lrt");
		cmd_append(&cmd, "-lpthread");

#elif defined(_WIN32)
		cmd_append(&cmd, "-DNOMINMAX");
		cmd_append(&cmd, "-lopengl32");
		cmd_append(&cmd, "-lgdi32");
		cmd_append(&cmd, "-lwinmm");
		cmd_append(&cmd, "-luser32");
		cmd_append(&cmd, "-lshell32");
		cmd_append(&cmd, "-lws2_32");
		cmd_append(&cmd, "-lcrypt32");
		cmd_append(&cmd, "-lbcrypt");
		cmd_append(&cmd, "-ladvapi32");
		cmd_append(&cmd, "-lole32");
		cmd_append(&cmd, "-lavrt");
#endif

		cmd_append(&cmd, "-Wno-unused-function");
		cmd_append(&cmd, "-Wno-unused-variable");
		cmd_append(&cmd, "-Wno-unused-parameter");
		cmd_append(&cmd, "-Wno-missing-field-initializers");
		cmd_append(&cmd, "-Wno-unused-value");
		cmd_append(&cmd, "-Wno-writable-strings");
		cmd_append(&cmd, "-Wno-unused-command-line-argument");
		cmd_append(&cmd, "-Wno-missing-braces");

		cmd_append(&cmd, "-o", BUILD_EXECUTABLE);
		if (!cmd_run(&cmd)) { return 1; }
	}

	if (debug && run) { cmd_append(&cmd, debugger); }

#if defined(__APPLE__)
	if (compile) {
		// Pfade sauber als Variablen definieren
		const char *app_dir = BUILD_FOLDER "chess-engine-battel.app";
		const char *contents_dir = BUILD_FOLDER "chess-engine-battel.app/Contents";
		const char *macos_dir = BUILD_FOLDER "chess-engine-battel.app/Contents/MacOS";
		const char *plist_path = BUILD_FOLDER "chess-engine-battel.app/Contents/Info.plist";
		const char *pkginfo_path = BUILD_FOLDER "chess-engine-battel.app/Contents/PkgInfo";

#ifdef __APPLE__

		const char *dysm_path = BUILD_FOLDER "chess-engine-battel.app/Contents/MacOS/chess-engine-battel.dSYM";
#endif

		cmd_append(&cmd, "rm", "-rf", app_dir);

		if (!cmd_run(&cmd)) { return 1; }

		// 1. Verzeichnisstruktur erstellen
		if (!mkdir_if_not_exists(app_dir)) { return 1; }
		if (!mkdir_if_not_exists(contents_dir)) { return 1; }
		if (!mkdir_if_not_exists(macos_dir)) { return 1; }

		// 4. Dein Icon-Verzeichnis kopieren
		if (!copy_directory_recursively("assets", BUILD_FOLDER "chess-engine-battel.app/Contents/Resources")) { return 1; }

		// 2. Executable in das MacOS-Verzeichnis kopieren
		if (!copy_file(BUILD_EXECUTABLE, BUILD_FOLDER "chess-engine-battel.app/Contents/MacOS/chess-engine-battel")) { return 1; }

		// 3. DebugInfo
		if (debug) {
#ifdef __APPLE__
			cmd_append(&cmd, "dsymutil", "-o", dysm_path, BUILD_EXECUTABLE);
			if (!cmd_run(&cmd)) { return 1; }

#endif
		}

		const char *debug_plist_entry = debug ? "   <key>CSResourcesFileMapped</key>\n"
		                                        "      <true/>\n"
		                                        "   <key>NSAppSleepDisabled</key>\n"
		                                        "      <true/>\n"
		                                      : "";

		// 4. Info.plist schreiben (Fehler aus dem Originalcode behoben)
		//        "   <key>DTCompiler</key>\n"
		//        "      <string>com.apple.compilers.llvm.clang.1_0 </string>\n"
		const char *plist_content = temp_sprintf(
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
		    "      <string>" VERSION
		    "</string>\n"
		    "   <key>CFBundleSupportedPlatforms</key>\n"
		    "      <array>\n"
		    "         <string>MacOSX</string>\n"
		    "      </array>\n"
		    "   <key>CFBundleVersion</key>\n"
		    "      <string>" VERSION
		    "</string>\n"
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
		    "      <string>Copyright © 2026 comboom.sucht. Alle Rechte "
		    "vorbehalten.</string>\n"
		    "%s"
		    "   <key>UIAppFonts</key>\n"
		    "      <array>\n"
		    "         <string>CaskaydiaCoveNerdFontPropo-Bold.ttf</string>\n"
		    "         <string>CaskaydiaCoveNerdFontPropo-Italic.ttf</string>\n"
		    "         <string>CaskaydiaCoveNerdFontPropo-Regular.ttf</string>\n"
		    "      </array>\n"
		    "   </dict>\n"
		    "   </plist>",
		    debug_plist_entry);

		const char *pkginfo_content = "\x41\x50\x50\x4C\x3f\x3f\x3f\x3f";

		if (!write_entire_file(plist_path, plist_content, strlen(plist_content))) {
			nob_log(ERROR, "Konnte Info.plist nicht erstellen: %s", plist_path);
			return 1;
		};

		if (!write_entire_file(pkginfo_path, pkginfo_content, strlen(pkginfo_content))) {
			nob_log(ERROR, "Konnte PkgInfo nicht erstellen: %s", pkginfo_path);
			return 1;
		};
	}
#else
	if (compile) {
		if (!copy_directory_recursively("assets", BUILD_FOLDER "assets")) { return 1; }
	}
#endif

	if (install) {
#ifdef __APPLE__
		if (!nob_copy_directory_recursively(BUILD_FOLDER "chess-engine-battel.app", temp_sprintf("%s/chess-engine-battel.app", install))) {
			return 1;
		}
#else

		if (!mkdir_if_not_exists(install)) { return 1; }
		if (!copy_directory_recursively(BUILD_FOLDER "assets", temp_sprintf("%s/assets", install))) { return 1; }
		if (!copy_file(BUILD_EXECUTABLE, temp_sprintf("%s/%s", install, EXECUTABLE))) { return 1; }
#endif
	}

	if (run) {
#ifdef __APPLE__
		set_current_dir(BUILD_FOLDER "chess-engine-battel.app/Contents/MacOS");
#endif
		cmd_append(&cmd, EXECUTABLE);
		if (!cmd_run(&cmd)) { return 1; }
	}

	return 0;
}
