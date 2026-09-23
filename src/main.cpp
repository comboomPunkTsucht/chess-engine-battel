#include <stdio.h>
#include <stdlib.h>

#define NOB_IMPLEMENTATION
#include "nob.h"
#include "nob_addons.h"

#define FLAG_IMPLEMENTATION
#include "flag.h"

#define HT_IMPLEMENTATION
#include "ht.h"

int main(int argc, char **argv) {
  nob_set_log_handler(nob_cancer_log_handler);
  UNUSED(argc);
  UNUSED(argv);
  nob_log(INFO, "Hello, World!");
  return 0;
}
