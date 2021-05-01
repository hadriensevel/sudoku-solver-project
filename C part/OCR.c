#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "DigitBitmap.h"

#define CELLSIZE 44

int readCell(uint8_t *cell, const char *cellPath);
int isCellEmpty(uint8_t *cell, int seuil);
int isNumber(uint8_t *cell, int chiffre, int pixelDeDepart);
uint8_t getPixelBitmap(int chiffre, int ligne, int colonne);

int main(int argc, const char *argv[]) {
  /* if (argc != 12) {
    printf("Erreur: mauvais paramètres."); TODO: Récupérer les arguments et les vérifier
    return 0;
  } */

  const char cellPath[100] = "Bitmaps\\bbox_empty.bin"; // TODO: remplacer par argument

  uint8_t *cell = malloc(CELLSIZE * CELLSIZE * sizeof(uint8_t));

  // Lit le fichier binaire, enregistre les pixels dans le tableau cell et continue si pas d'erreur
  if (!readCell(cell, cellPath)) {

    // Vérifie si le nombre de pixels blancs est supérieur au seuil donné, donc si la case est vide
    if (isCellEmpty(cell, 98)) { // TODO: remplacer seuil par argument
      printf("L'image est une case vide.\n");
    }

    else {
      for (int i = 0; i < 9; i++) {

      }
    }

  };

  free(cell);
  cell = NULL;

  return 0;
}

int readCell(uint8_t *cell, const char *cellPath) {

  // Déclaration des variables
  const uint32_t nombrePixels = CELLSIZE * CELLSIZE;
  uint32_t largeur;
  uint32_t hauteur;

  // Ouverture du fichier binaire
  FILE *fp = fopen(cellPath, "rb");
  if (fp == NULL) {
    perror("Erreur lors de l'ouverture du fichier binaire");
    return 1;
  }

  // Lecture et vérification de la largeur et de la hauteur de l'image
  size_t r1 = fread(&largeur, sizeof largeur, 1, fp);
  size_t r2 = fread(&hauteur, sizeof hauteur, 1, fp);
  if (!(r1 && r2)) {
    printf("Erreur: problème lors de la lecture des dimensions de l'image.\n");
    return 1;
  }
  else if (hauteur != 44 || largeur != 44) {
    printf("Erreur: mauvaise largeur et/ou hauteur de l'image.\n");
    return 1;
  }

  // Verification du nombre de pixels dans le fichier binaire
  long begin_pixels = ftell(fp);
  fseek(fp,0,SEEK_END);
  long end_pixels = ftell(fp);
  fseek(fp,begin_pixels,SEEK_SET);
  if (end_pixels-begin_pixels != 1936) {
    printf("Erreur: mauvais nombre de pixels dans l'image.\n");
    return 1;
  }

  // Lecture des pixels de l'image
  size_t pixelsLus = fread(cell, sizeof cell[0], nombrePixels, fp);
  if (pixelsLus != nombrePixels) {
    printf("Erreur: pas assez de pixels dans l'image.\n");
    return 1;
  }

  // Fermeture du fichier binaire
  if (fclose(fp)) perror("Erreur lors de la fermeture du fichier");

  // Quitte la fonction sans erreur
  return 0;
}

uint8_t getPixelBitmap(int chiffre, int ligne, int colonne) {
  return (DigitBitmap[chiffre - 1][ligne] >> (DigitBitmapWidth - colonne)) & 1;
}

int isCellEmpty(uint8_t *cell, int seuil) {

  // Déclaration des variables
  int nombrePixelsBlancs = 0;
  const int nombrePixels = CELLSIZE * CELLSIZE;

  // Boucle sur tous les pixels de l'image et compte les pixels blancs (octets nuls)
  for (int i = 0; i < nombrePixels; i++) {
    if (cell[i] == 0) nombrePixelsBlancs++;
  }

  // Si le pourcentage de pixels blancs est suprérieur au seuil donné retourne 1 sinon 0
   return (nombrePixelsBlancs * 100 / nombrePixels >= seuil) ? 1 : 0;
}