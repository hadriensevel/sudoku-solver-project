#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "DigitBitmap.h"

#define CELLSIZE 44

int readCell(uint8_t *cell, const char *cellPath);
int isCellEmpty(uint8_t *cell, int seuil);
void findNumber(uint8_t *cell, float *ratios);
float getRatioCommonPixels(uint8_t *cell, int chiffre, int startPixel);
uint8_t getPixelBitmap(int chiffre, int ligne, int colonne);

int main(int argc, const char *argv[]) {
  /* if (argc != 12) {
    printf("Erreur: mauvais paramètres."); TODO: Récupérer les arguments et les vérifier
    return 0;
  } */

  const char cellPath[100] = "D:\\Documents\\EPFL\\BA4-1\\Programmation\\sudoku-solver-project\\C part\\Bitmaps\\bbox_5.bin"; // TODO: remplacer par argument

  // Créer le tableau qui va contenir les pixels du fichier
  uint8_t *cell = malloc(CELLSIZE * CELLSIZE * sizeof(uint8_t));

  // Créer le tableau qui va contenir le ratio de similitude le plus élévé pour chaque chiffre
  float *ratios = malloc(9 * sizeof(float));

  // Lit le fichier binaire, enregistre les pixels dans le tableau cell et continue si pas d'erreur
  if (!readCell(cell, cellPath)) {

    // Vérifie si le nombre de pixels blancs est supérieur au seuil donné, donc si la case est vide
    if (isCellEmpty(cell, 98)) { // TODO: remplacer seuil par argument
      printf("L'image est une case vide.\n");
    }

    else {
      findNumber(cell, ratios);
    }

  };

  for (int i = 0; i < 9; i++) {
    printf("%d: %0.4f %%\n", i+1, ratios[i]);
  }

  free(cell);
  free(ratios);
  cell = NULL;
  ratios = NULL;

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

float getRatioCommonPixels(uint8_t *cell, int chiffre, int startPixel) {

  // Déclaration des variables
  int nombrePixelsCommuns = 0;

  // Boucle sur les lignes du bitmap
  for (int i = 0; i < 24; i++) {

    //Boucles sur les colonnes du bitmap
    for (int j = 0; j < 16; j++) {

      if (getPixelBitmap(chiffre, i, j) == cell[j + startPixel + CELLSIZE * i]) nombrePixelsCommuns++;

    }
  }

  // Retourne le ratio de pixels communs
  return nombrePixelsCommuns * 100.0 / (24.0 * 16.0);
}

void findNumber(uint8_t *cell, float *ratios) {

  // Boucle sur les 9 chiffres
  for (int i = 1; i <= 9; i++) {

    float temp = 0;
    ratios[i] = 0;

    // Boucle sur les 560 déplacements que le bitmap doit effectuer
    for (int j = 0; j < 560; j++) {

      // Détermine la ligne et la colonne du pixel de départ de la cellule
      // 28 -> 28 déplacements en largeur car 44 (taille cellule) - 16 (taille bitmap) = 28
      int ligneStartPixel = (j - (j % 28)) / 28;
      int colonneStartPixel = j % 28;

      // Calcul le pixel de départ pour le tableau des pixels de la cellule
      int startPixel = 44 * ligneStartPixel + colonneStartPixel;

      // Calcul le ratio de pixels communs pour la position courante du bitmap sur la cellule
      temp = getRatioCommonPixels(cell, i, startPixel);

      // Si le ratio obtenu est plus grand que précédent, l'enregistre dans le tableau des ratios
      if (temp > ratios[i-1]) {
        ratios[i-1] = temp;
        printf("Chiffre: %d, ligne: %d, colonne: %d\n", i, ligneStartPixel, colonneStartPixel);
        }

    }
  }

  return;
}