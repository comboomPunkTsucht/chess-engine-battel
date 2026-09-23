/**
 * @file assets2c.cpp
 * @author mcpeaps_HD<mcpeaps_HD@mcpeapsUnterstrichHD.dev>
 * @brief
 * @version 0.1
 * @date 2026-09-23
 *
 * @copyright Copyright (c) 2026
 *
 * this is the assets2c tool, which is used to convert assets into C code that
 * can be compiled staticallly into the main executable.
 */

#include "assets2c.h"

int main(int argc, char **argv) {
  addon_init_logging();
  UNUSED(argc);
  UNUSED(argv);
  nob_log(INFO, "Hello, World!");
  return 0;
}
