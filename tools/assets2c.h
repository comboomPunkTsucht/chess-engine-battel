#pragma once
#include <string>

namespace Assets2C {
/**
 * Konvertiert alle Dateien in 'input_dir' in C-Arrays und schreibt sie in
 * 'output_header'.
 * @param input_dir Der Pfad zum Assets-Ordner (z.B. "assets")
 * @param output_header Der Pfad zur generierten Header-Datei (z.B.
 * "build/assets/assets.h")
 * @return true bei Erfolg, false bei Fehlern.
 */
bool generate_header(const std::string &input_dir,
                     const std::string &output_header);
} // namespace Assets2C
