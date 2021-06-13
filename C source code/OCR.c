/************************************************
 * File: OCR.c
 * Created on 2.4.2021
 * Author: Hadrien Sevel
 ************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "DigitBitmap.h"

#define NUMBER_OF_DIGITS 9
#define LOWER_BOUND 5
#define UPPER_BOUND 100

uint8_t *readCell(const char *cellPath, uint32_t *largeur, uint32_t *hauteur);
int isCellEmpty(uint8_t *cell, int seuil, uint32_t *largeur, uint32_t *hauteur);
void findNumber(uint8_t *cell, int *seuils, uint32_t *largeur, uint32_t *hauteur);
float getRatioCommonPixels(uint8_t *cell, int chiffre,  int offsetY, int offsetX, uint32_t *largeur);
uint8_t getPixelBitmap(int chiffre, int ligne, int colonne);


int main(int argc, const char *argv[]) {

  /* Vérifie le nombre d'arguments donnés */
  if (argc != (NUMBER_OF_DIGITS + 3)) {
    fprintf(stderr, "Wrong number of parameters.\n");
    return 1;
  }

  /* Créer le tableau qui va contenir les seuils */
  int *seuils = malloc((NUMBER_OF_DIGITS + 1) * sizeof(int));

  /* Vérifie les seuils et les enregistre dans le tableau */
  for (int i = 0; i < (NUMBER_OF_DIGITS + 1); i++) {
    int seuil = atoi(argv[i+2]);
    if (seuil < LOWER_BOUND || seuil > UPPER_BOUND) {
      fprintf(stderr, "Wrong value for one or more thresholds.\n");
      return 1;
    } else {
      seuils[i] = seuil;
    }
  }

  /* Créer les variables qui vont contenir la hauteur et la largeur de l'image */
  uint32_t largeur = 0;
  uint32_t hauteur = 0;

  /* Lit le fichier binaire, enregistre les pixels dans le tableau cell et continue si pas d'erreur */
  uint8_t *cell = readCell(argv[1], &largeur, &hauteur);
  if (cell != NULL) {

    findNumber(cell, seuils, &largeur, &hauteur);

  } else return 1;

  /* Libère la mémoire utilisée par les tableaux */
  free(cell);
  free(seuils);

  /* Quitte le programme sans erreur */
  return 0;
}

uint8_t *readCell(const char *cellPath, uint32_t *largeur, uint32_t *hauteur) {

/* La fonction readCell ouvre le fichier binaire, vérifie les dimensions de l'image,
 * vérifie le nombre de pixels, enregistre la valeur des pixels dans un tableau, ferme
 * le fichier binaire et retourne le pointeur du tableau.
 * Arguments:
 *    - char *cellPath: chemin du fichier binaire à ouvrir
 *    - uint32_t *largeur & *hauteur: pointeurs sur 2 variables pour enregistrer la
 *      largeur et la hauteur de l'image
 */

  uint32_t nombrePixels;

  /* Ouverture du fichier binaire */
  FILE *fp = fopen(cellPath, "rb");
  if (fp == NULL) {
    perror("Error opening the binary file");
    return NULL;
  }

  /* Lecture et vérification de la largeur et de la hauteur de l'image */
  size_t r1 = fread(largeur, sizeof(uint32_t), 1, fp);
  size_t r2 = fread(hauteur, sizeof(uint32_t), 1, fp);
  if (!(r1 && r2)) {
    fprintf(stderr, "Unable to read cell dimensions.\n");
    if (fclose(fp)) perror("Error when closing the binary file");
    return NULL;
  }
  /* Hauteur doit être comprise dans [24..100] et largeur dans [16..100] */
  else if (*hauteur < DigitBitmapHeight || *hauteur > UPPER_BOUND || *largeur < DigitBitmapWidth || *largeur > UPPER_BOUND) {
    fprintf(stderr, "Wrong width and/or height of the cell.\n");
    if (fclose(fp)) perror("Error when closing the binary file");
    return NULL;
  }

  /* Calcul du nombre de pixels dans l'image */
  nombrePixels = (*largeur) * (*hauteur);

  /* Allocation de la mémoire pour le tableau cell */
  uint8_t *cell = malloc(nombrePixels * sizeof(uint8_t));

  /* Lecture des pixels de l'image et vérification du nombre de pixels lus */
  size_t pixelsLus = fread(cell, sizeof cell[0], nombrePixels, fp);
  if (pixelsLus != nombrePixels) {
    fprintf(stderr, "Not enough pixels in the cell.\n");
    if (fclose(fp)) perror("Error when closing the binary file");
    return NULL;
  } else if (getc(fp) != EOF) {
    /* Si trop de pixels, affiche un avertissement mais continue */
    fprintf(stdout, "Warning: too many pixels in the image, additional pixels ignored.\n");
  }

  /* Fermeture du fichier binaire*/
  if (fclose(fp)) perror("Error when closing the binary file");

  /* Quitte la fonction sans erreur et retourne le pointeur cell */
  return cell;
}

uint8_t getPixelBitmap(int chiffre, int ligne, int colonne) {

/* La fonction getPixelBitmap retourne la valeur du pixel du bitmap pour le chiffre,
 * la ligne et la colonne donnés.
 */

  return (DigitBitmap[chiffre - 1][ligne] >> (DigitBitmapWidth - colonne - 1)) & 1;
}

int isCellEmpty(uint8_t *cell, int seuil, uint32_t *largeur, uint32_t *hauteur) {

/* La fonction isCellEmpty compte le nombre de pixels blancs dans la case et compare le pourcentage obtenu
 * avec le seuil donnée en argument: retourne 1 si le pourcentage est supérieur au seuil (case vide) et
 * retourne 0 si le pourcentage est inférieur au seuil
 */

  int nombrePixelsBlancs = 0;
  const int nombrePixels = (*largeur) * (*hauteur);

  /* Boucle sur tous les pixels de l'image et compte les pixels blancs (octets nuls) */
  for (int i = 0; i < nombrePixels; i++) {
    if (cell[i] == 0) nombrePixelsBlancs++;
  }

  /* Si le pourcentage de pixels blancs est supérieur au seuil donné retourne 1 sinon 0 */
   return (nombrePixelsBlancs * 100 / nombrePixels >= seuil) ? 1 : 0;
}

float getRatioCommonPixels(uint8_t *cell, int chiffre, int offsetY, int offsetX, uint32_t *largeur) {

/* La fonction getRatioCommonPixels compte le nombre de pixels communs (noirs & blancs) entre le bitmap
 * et la case et calcule le ratio. Pour cela, elle boucle sur tous les pixels du bitmap et les compare
 * avec les pixels de la case en tenant compte du décalage du bitmap sur la case (offsetX & offsetY).
 * Pour obtenir le pixel du bitmap, elle appelle le fonction getPixelBitmap.
 * Arguments:
 *    Arguments:
 *    - uint8_t *cell: tableau contenant les pixels de l'image
 *    - int chiffre: chiffre à utiliser pour le bitmap
 *    - int offsetY & offsetX: décalage vertical et horizontal du bitmap sur la case
 *    - uint32_t *largeur: largeur de la case
 */

  int nombrePixelsCommuns = 0;

  /* Boucle sur les lignes du bitmap */
  for (int i = 0; i < DigitBitmapHeight; i++) {

    /* Boucles sur les colonnes du bitmap */
    for (int j = 0; j < DigitBitmapWidth; j++) {

      if (getPixelBitmap(chiffre, i, j) == cell[(i + offsetY - 1) * (*largeur) + j + offsetX]) nombrePixelsCommuns++;

    }
  }

  /* Retourne le ratio de pixels communs */
  return nombrePixelsCommuns * 100.0 / (DigitBitmapHeight * DigitBitmapWidth);
}

void findNumber(uint8_t *cell, int *seuils, uint32_t *largeur, uint32_t *hauteur) {

/* La fonction findNumber appelle la fonction isCellEmpty pour savoir si la case est vide, sinon
 * boucle sur les chiffres du sudoku, sur les déplacements verticaux et horizontaux que le bitmap
 * doit effectuer sur l'image et appelle à chaque fois la fonction getRatioCommonPixels pour obtenir
 * le ratio de pixels communs entre le bitmap et l'image. Enfin, elle enregistre le ratio max et
 * écrit le chiffre correspondant et son ratio dans le fichier "CellValue.txt".
 * Si isCellEmpty n'a pas retourné la case comme étant vide mais que aucun ratio max n'a dépassé le
 * seuil associé, alors la case est considérée vide (notée 0 dans le fichier "CellValue.txt").
 * Arguments:
 *    - uint8_t *cell: tableau contenant les pixels de l'image
 *    - int *seuils: tableau contenant les seuils
 *    - uint32_t *largeur & *hauteur: pointeurs sur la hauteur et la largeur de l'image
 */

  int number = 0;
  float ratio_max = 0;

  if (!(isCellEmpty(cell, seuils[0], largeur, hauteur))) {

    /* Boucle sur les 9 chiffres */
    for (int i = 1; i <= NUMBER_OF_DIGITS; i++) {

      float ratio = 0;

      /* Boucle sur les déplacements verticaux que le bitmap doit effectuer */
      for (int j = 0; j < (*hauteur - DigitBitmapHeight); j++) {

        /* Boucle sur les déplacements horizontaux que le bitmap doit effectuer */
        for (int k = 0; k < (*largeur - DigitBitmapWidth); k++) {

          /* Calcul le ratio de pixels communs pour la position courante du bitmap sur la cellule */
          ratio = getRatioCommonPixels(cell, i, j, k, largeur);

          /* Si le ratio obtenu est supérieur au seuil et supérieur au ratio max, on enregistre
             le ratio comme ratio max et le chiffre courant comme chiffre reconnu */
          if (ratio > seuils[i] && ratio > ratio_max) {
            ratio_max = ratio;
            number = i;
          }

        }
      }
    }
  }

  /* Ouvre ou crée le fichier "CellValue.txt" */
  FILE *fp = fopen("CellValue.txt", "w");
  if (fp == NULL) {
    perror("Error when opening/creating the file CellValue.txt");
    return;
  }
  /* Écrit le chiffre reconnu et son ratio max (0 si case vide) */
  fprintf(fp,"d:'%d', %0.4f%%\n", number, ratio_max);
  /* Ferme le fichier */
  if (fclose(fp)) perror("Error when closing the file CellValue.txt");

  /* Quitte la fonction sans erreur */
  return;
}