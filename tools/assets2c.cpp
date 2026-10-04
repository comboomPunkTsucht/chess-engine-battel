#include "assets2c.h"

#define NOB_IMPLEMENTATION
#include "nob_addons.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace Assets2C {

// Hilfsfunktion: Macht aus Dateinamen gültige C++ Variablennamen
// z.B. "images/logo.png" -> "images_logo_png"
std::string sanitize_name(std::string name) {
  for (char &c : name) {
    if (!isalnum(c)) { c = '_'; }
  }
  return name;
}

bool generate_header(const std::string &input_dir,
                     const std::string &output_header) {
  std::ofstream out(output_header);
  if (!out) {
    std::cerr << "Fehler: Konnte Ausgabedatei nicht oeffnen: " << output_header
              << "\n";
    return false;
  }

  out << "// AUTOMATISCH GENERIERT DURCH assets2c\n";
  out << "#pragma once\n";
  out << "#include <cstddef>\n\n";
  out << "namespace Assets {\n\n";

  if (!fs::exists(input_dir) || !fs::is_directory(input_dir)) {
    nob_log(ERROR, "Asset-Verzeichnis '%s' existiert nicht.",
            input_dir.c_str());
    return true;
  }

  // Iteriere durch alle Dateien im Ordner (auch Unterordner)
  for (const auto &entry : fs::recursive_directory_iterator(input_dir)) {
    if (entry.is_regular_file()) {
      std::ifstream in(entry.path(), std::ios::binary);
      if (!in) { continue; }

      // Gesamte Datei in Buffer lesen
      std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(in)),
                                        {});

      // Relativen Pfad als Basis für den Variablennamen nutzen
      std::string rel_path = fs::relative(entry.path(), input_dir).string();
      std::string var_name = sanitize_name(rel_path);

      // Array schreiben (inline verhindert Multiple-Definition Fehler beim
      // Linken)
      out << "    inline const unsigned char " << var_name
          << "[] = {\n        ";
      for (size_t i = 0; i < buffer.size(); ++i) {
        out << "0x" << std::hex << std::setw(2) << std::setfill('0')
            << (int)buffer[i] << ", ";
        if ((i + 1) % 16 == 0) {
          out << "\n        "; // Zeilenumbruch alle 16 Bytes
        }
      }
      out << "\n    };\n";
      // Größe speichern
      out << "    inline const size_t " << var_name << "_size = " << std::dec
          << buffer.size() << ";\n\n";
    }
  }

  out << "} // namespace Assets\n";
  return true;
}
} // namespace Assets2C

int main(int argc, char **argv) {

  addon_init_logging();
  // Standard-Pfade
  std::string input_dir = "assets";
  std::string output_dir = "build/assets";
  std::string output_file = "build/assets/assets.h";

  // Erlaube Überschreiben per Argumente (CLI)
  if (argc > 1) { input_dir = argv[1]; }
  if (argc > 2) { output_file = argv[2]; }
  if (argc > 3) { output_dir = argv[3]; }

  // Stelle sicher, dass der Ausgabeordner (z.B. build/assets/) existiert
  if (!mkdir_if_not_exists(output_dir.c_str())) {
    nob_log(ERROR, "Konnte Ausgabeordner '%s' nicht erstellen.",
            output_dir.c_str());
    return 1;
  }

  nob_log(INFO, "Generiere Header aus '%s' nach '%s'...", input_dir.c_str(),
          output_file.c_str());

  if (Assets2C::generate_header(input_dir, output_file)) {
    nob_log(INFO, "Header-Datei '%s' erfolgreich generiert.",
            output_file.c_str());
    return 0;
  } else {
    return 1;
  }
}
