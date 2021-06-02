#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "DigitBitmap.h"

#define CELLSIZE 44
#define NUMBER_OF_DIGITS 9

int readCell(uint8_t *cell, const char *cellPath);
int isCellEmpty(uint8_t *cell, int seuil);
void findNumber(uint8_t *cell, int *seuils);
float getRatioCommonPixels(uint8_t *cell, int chiffre,  int offsetY, int offsetX);
uint8_t getPixelBitmap(int chiffre, int ligne, int colonne);

int main(int argc, const char *argv[]) {

  // Vérifie le nombre d'arguments donnés
  if (argc != 12) {
    fprintf(stderr, "Wrong number of parameters.\n");
    return 1;
  }

  // Créer le tableau qui va contenir les seuils
  int *seuils = malloc(10 * sizeof(int));

  // Vérifie les seuils et les enregistre dans le tableau
  for (int i = 0; i < 10; i++) {
    int seuil = atoi(argv[i+2]);
    if (seuil < 5 || seuil > 100) {
      fprintf(stderr, "Wrong value for one or more thresholds.\n");
      return 1;
    } else {
      seuils[i] = seuil;
    }
  }

  // Créer le tableau qui va contenir les pixels du fichier
  uint8_t *cell = malloc(CELLSIZE * CELLSIZE * sizeof(uint8_t));

  // Lit le fichier binaire, enregistre les pixels dans le tableau cell et continue si pas d'erreur
  if (!readCell(cell, argv[1])) {

    findNumber(cell, seuils);

  } else return 1;

  free(cell);
  free(seuils);
  cell = NULL;
  seuils = NULL;

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
    perror("Error opening the binary file");
    return 1;
  }

  // Lecture et vérification de la largeur et de la hauteur de l'image
  size_t r1 = fread(&largeur, sizeof largeur, 1, fp);
  size_t r2 = fread(&hauteur, sizeof hauteur, 1, fp);
  if (!(r1 && r2)) {
    fprintf(stderr, "Unable to read cell dimensions.\n");
    return 1;
  }
  else if (hauteur != CELLSIZE || largeur != CELLSIZE) {
    fprintf(stderr, "Wrong width and/or height of the cell.\n");
    return 1;
  }

  // Lecture des pixels de l'image
  size_t pixelsLus = fread(cell, sizeof cell[0], nombrePixels, fp);
  if (pixelsLus != nombrePixels) {
    fprintf(stderr, "Not enough pixels in the cell.\n");
    return 1;
  } /* else if (!feof(fp)) {
    printf("Attention: trop de pixels dans l'image, pixels suplémentaires ignorés.\n");
  } */ // TODO: vérifier que la lecture s'est terminée à la fin du fichier

  // Fermeture du fichier binaire
  if (fclose(fp)) perror("Error when closing the file");

  // Quitte la fonction sans erreur
  return 0;
}

uint8_t getPixelBitmap(int chiffre, int ligne, int colonne) {
  return (DigitBitmap[chiffre - 1][ligne] >> (DigitBitmapWidth - colonne - 1)) & 1;
}

int isCellEmpty(uint8_t *cell, int seuil) {

  // Déclaration des variables
  int nombrePixelsBlancs = 0;
  const int nombrePixels = CELLSIZE * CELLSIZE;

  // Boucle sur tous les pixels de l'image et compte les pixels blancs (octets nuls)
  for (int i = 0; i < nombrePixels; i++) {
    if (cell[i] == 0) nombrePixelsBlancs++;
  }

  // Si le pourcentage de pixels blancs est supérieur au seuil donné retourne 1 sinon 0
   return (nombrePixelsBlancs * 100 / nombrePixels >= seuil) ? 1 : 0;
}

float getRatioCommonPixels(uint8_t *cell, int chiffre, int offsetY, int offsetX) {

  // Déclaration des variables
  int nombrePixelsCommuns = 0;

  // Boucle sur les lignes du bitmap
  for (int i = 0; i < DigitBitmapHeight; i++) {

    //Boucles sur les colonnes du bitmap
    for (int j = 0; j < DigitBitmapWidth; j++) {

      if (getPixelBitmap(chiffre, i, j) == cell[(i + offsetY - 1) * CELLSIZE + j + offsetX]) nombrePixelsCommuns++;

    }
  }

  // Retourne le ratio de pixels communs
  return nombrePixelsCommuns * 100.0 / (DigitBitmapHeight * DigitBitmapWidth);
}

void findNumber(uint8_t *cell, int *seuils) {

  int number = 0;
  float ratio_max = 0;


  if (!(isCellEmpty(cell, seuils[0]))) {

    // Boucle sur les 9 chiffres
    for (int i = 1; i <= NUMBER_OF_DIGITS; i++) {
  
      float ratio = 0;
  
      // Boucle sur les déplacements verticaux que le bitmap doit effectuer
      for (int j = 0; j < (CELLSIZE - DigitBitmapHeight); j++) {
  
        // Boucle sur les déplacements horizontaux que le bitmap doit effectuer
        for (int k = 0; k < (CELLSIZE - DigitBitmapWidth); k++) {
  
          // Calcul le ratio de pixels communs pour la position courante du bitmap sur la cellule
          ratio = getRatioCommonPixels(cell, i, j, k);
  
          // Si le ratio obtenu est supérieur au seuil et supérieur au ratio max, on enregistre
          // le ratio comme ratio max et le chiffre courant comme chiffre reconnu
          if (ratio > seuils[i] && ratio > ratio_max) {
            ratio_max = ratio;
            number = i;
          }
  
        }
      }
    }
  }

  FILE *fp = fopen("CellValue.txt", "w");
  if (fp == NULL) {
    perror("Erreur lors de l'ouverture du fichier binaire");
    return;
  }
  fprintf(fp,"d:'%d', %0.4f%%\n", number, ratio_max);
  fclose(fp);

  return;
}