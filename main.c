#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compress(FILE *fin, char ArchiveName[200]);
void extract(FILE *fin, char FileName[200]);

int main(int argc, char* argv[]){
    FILE *fin;
    int i, j;
    char address[200];
    char FileName[200];
    char ArchiveName[200];
    for(i = 0; i < 200; i++){
      FileName[i] = NULL;
      ArchiveName[i] = NULL;
    }
    //Работа с консолью

      if (strcmp(argv[3], "-a") == 0){
        if (fopen(argv[1], "rb") != NULL){
          fin = fopen(argv[1], "rb");
          compress(fin, argv[2]);
        } else
            printf("\nCannot find file: %s ", argv[1]);
      } else if (strcmp(argv[3], "-e") == 0){
          if (fopen(argv[2], "rb") != NULL){
            fin = fopen(argv[2], "rb");
            extract(fin, argv[1]);
          } else
              printf("\nCannot find file: %s ", argv[2]);
        } else
            printf("\nUnknowKey, Try again: ");

    return 0;
}

void compress(FILE *fin, char ArchiveName[200]){
  char c;
  FILE *fout;
  fout = fopen(strncat(ArchiveName, ".vlt", 4), "w");
  fprintf(fout, "vlt\n");
  while (1){
    c = fgetc(fin);
    if (c == EOF) break;
    fprintf(fout, "%c", c);
  }
}

void extract(FILE *fin, char FileName[200]){
  char c;
  FILE *fout;
  fout = fopen(FileName, "w");
  char* test[4];
  fgets(test, 4, fin);
  if (strcmp(test, "vlt") == 0){
    while (1){
      c = fgetc(fin);
      if (c == EOF) break;
      fprintf(fout, "%c", c);
    }
  } else printf("Error... Wrong Format File...");
}

