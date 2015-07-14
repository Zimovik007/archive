#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.c"

void compress(FILE *fin, char ArchiveName[200], int fileCount);
void extract(FILE *fin, char FileName[200]);

int main(int argc, char* argv[]){
  FILE *fin, *fout;
  int i;
  char c;  
  //Работа с консолью
  if ((argc > 1) && (argc < 4)){
    printf("Error. Wrong Number Of Arguments");
    return 0;
  }
  if (argc == 1){
    printf("\n\nHello, bro. It's archivator. Developers: \n");
    printf("    Gusarov.v, Shchurov Al., Karandaev T.\n\n==================\n\n");
    printf("Look scheme comands:");
    printf("\n  [FileName] [ArchiveName] -a    |Archive");
    printf("\n  [FileName] [ArchiveName] -e    |Extract\n\n==================\n");
    return 0;
  }
    i = 1;
    int* lengths = malloc(sizeof(int) * (argc - 2));
    fout = fopen("temp.txt", "w");
    while (i < argc - 2){
      fin = fopen(argv[i], "r");
      while (fscanf(fin, "%c", &c) != EOF){
        lengths[i]++;
        fprintf(fout, "%c", c);
      }
      i++;
    }

  if (strcmp(argv[argc - 1], "-a") == 0){
    if (fopen("temp.txt", "rb") != NULL){
      fin = fopen("temp.txt", "rb");
      compress(fin, argv[argc - 2], argc-3);
    } else
        printf("\nCannot find file: %s ", argv[1]);
  } else if (strcmp(argv[argc - 1], "-e") == 0){
      if (fopen(argv[argc - 2], "rb") != NULL){
        fin = fopen(argv[argc - 2], "rb");
        extract(fin, argv[argc - 3]);
      } else
          printf("\nCannot find file: %s ", argv[2]);
    } else
        printf("\nUnknow Key, Try again: ");

  return 0;
}

void compress(FILE *fin, char ArchiveName[200], int fileCount){
  compress_huffman(fin, ArchiveName, fileCount);
}

void extract(FILE *fin, char FileName[200]){
  char c;
  FILE *fout;
  fout = fopen(FileName, "w");
  char test[4];
  fgets(test, 4, fin);
  if (strcmp(test, "vlt") == 0){
    while (1){
      c = fgetc(fin);
      if (c == EOF) break;
      fprintf(fout, "%c", c);
    }
  } else printf("Error... Wrong Format File...");
}

