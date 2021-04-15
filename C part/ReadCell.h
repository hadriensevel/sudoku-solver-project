#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NOMBREPIXELS 1936

uint8_t *readCell(const char *cellPath) {

  // Déclaration des variables
  uint32_t largeur;
  uint32_t hauteur;
  uint8_t *cell = malloc(NOMBREPIXELS * sizeof(uint8_t));

  // Ouverture du fichier binaire
  FILE *fp = fopen(cellPath, "rb");
  if (fp == NULL) {
    perror("Erreur lors de l'ouverture du fichier binaire");
    return NULL;
  }

  // Lecture et vérification de la largeur et de la hauteur de l'image
  size_t r1 = fread(&largeur, sizeof largeur, 1, fp);
  size_t r2 = fread(&hauteur, sizeof hauteur, 1, fp);
  if (!(r1 && r2)) {
    printf("Erreur: problème lors de la lecture des dimensions de l'image.\n");
    return NULL;
  }
  else if (hauteur != 44 || largeur != 44) {
    printf("Erreur: mauvaise largeur et/ou hauteur de l'image.\n");
    return NULL;
  }

  // Verification du nombre de pixels dans le fichier binaire
  long begin_pixels = ftell(fp);
  fseek(fp,0,SEEK_END);
  long end_pixels = ftell(fp);
  fseek(fp,begin_pixels,SEEK_SET);
  if (end_pixels-begin_pixels != 1936) {
    printf("Erreur: mauvais nombre de pixels dans l'image.\n");
    return NULL;
  }

  // Lecture des pixels de l'image
  size_t pixelsLus = fread(cell, sizeof cell[0], NOMBREPIXELS, fp);
  if (pixelsLus != NOMBREPIXELS) {
    printf("Erreur: pas assez de pixels dans l'image.\n");
    return NULL;
  }

  // Fermeture du fichier binaire
  if(fclose(fp)) perror("Erreur lors de la fermeture du fichier");

  // Retourne le pointeur cell
  return cell;
}