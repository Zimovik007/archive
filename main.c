#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extract_huff.h"
#include "compress.h"


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
  compress_huffman(fin, ArchiveName);
}

void extract(FILE *fin, char FileName[200]){
  int i, j;
  char test1[17], test2[10], test3;
  fgets(test1, 17, fin);
  fscanf(fin, "\n");
  fgets(test2, 10, fin);
  fscanf(fin, "\n");
  if (strcmp(test1, "UPA file archive") != 0){printf("Wrong format file..."); return;}
  if (strcmp(test2, "sign: UPA") != 0) {printf("Wrong format file..."); return;}
  char alg_id[5];
  fgets(alg_id, 5, fin);
  fscanf(fin, "\n");
  fscanf(fin, "%c\n", &test3);
  if (test3 != '1') {printf("Wrong type extract... Sorry"); return;}
  char FileCount;
  fscanf(fin, "%c", &FileCount);
  char** FileNames;
  char* FileLenghts;
  FileNames = malloc(sizeof(char) * FileCount);
  FileLenghts = malloc(sizeof(char) * FileCount);
  for(i = 0; i < FileCount; i++){
    fscanf(fin, "%c\n", &FileLenghts[i]);
    FileNames[i] = malloc(sizeof(char) * FileLenghts[i]);
    for(j = 0; j < FileLenghts[i]; j++)
      fscanf(fin, "%c", &FileNames[i][j]);
    fscanf(fin, "\n");
  }
  return;
  FILE *fout = fopen(FileName, "w");
  if (strcmp(alg_id, "HUFF")) extract_huffman(fin, fout);
  fclose(fout);
}

