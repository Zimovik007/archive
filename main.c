#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.c"

typedef struct entries{
  unsigned int FileLength;
  unsigned int SizePached;
  unsigned int SizeOriginal;
  unsigned int Attributes;
  unsigned int Time_Created;
  unsigned int Time_Modified;
  char FileName[200];
} enter;

void compress(FILE *fin, char ArchiveName[200], int fileCount);
void extract(FILE *fin, char FileName[200]);

int main(int argc, char* argv[]){
  FILE *fin, *fout;
  int i, j;
  char c;
  //Работа с консолью
  if ((argc > 1) && (argc < 4)){
    printf("Error. Wrong Number Of Arguments");
    return 0;
  }
  if (argc == 1){
    printf("\n\nHello, bro. It's archivator. Developers: \n");
    printf("    Gusarov.v, Schyrov Al., Karandaev T.\n\n==================\n\n");
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
  int i, j, d, FileCount;
  enter* Files;
  char test1[17], test2[10], alg_id[5];
  fgets(test1, 17, fin); fscanf(fin, "\n");
  fgets(test2, 10, fin); fscanf(fin, "\n");
  fgets(alg_id, 5, fin); fscanf(fin, "\n");
  fscanf(fin, "%d", &d);
  if (strcmp(test1, "UPA file archive") != 0){printf("Wrong Format File!\n"); return;}
  if (strcmp(test2, "sign: UPA") != 0){printf("Wrong Format File!\n"); return;}
  if (d != 1){printf("Wrong Type Extract... Sorry... I don't extract this file\n"); return;}
  fscanf(fin,"%d\n", &FileCount);
  Files = malloc(sizeof(enter) * FileCount);
  for(i = 0; i < FileCount; i++){
    fscanf(fin, "%d\n", &Files[i].FileLength);
    fscanf(fin, "%s\n", &Files[i].FileName);
    fscanf(fin, "%d\n", &Files[i].SizePached);
    fscanf(fin, "%d\n", &Files[i].SizeOriginal);
    fscanf(fin, "%d\n", &Files[i].Attributes);
    fscanf(fin, "%d\n", &Files[i].Time_Created);
    fscanf(fin, "%d\n", &Files[i].Time_Modified);
  }
  //if(strcmp(alg_id, "HUFF") == 0) extract_huffman(fin);
}

