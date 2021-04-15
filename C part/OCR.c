#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "DigitBitmap.h"
#include "ReadCell.h"


int main(int argc, const char *argv[]) {
  /* if (argc != 12) {
    printf("Erreur: mauvais paramètres.");
    return 0;
  } */

  uint8_t *cell = readCell("Bitmaps\\bbox_test3.bin");
  if (cell == NULL) {
    exit(1);
  }

  free(cell);
  return 0;
}