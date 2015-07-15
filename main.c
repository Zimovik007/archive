#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <windows.h>
#include "compress.c"

typedef struct entries{
  unsigned int FileLength;
  unsigned int SizePached;
  unsigned int SizeOriginal;
  unsigned int Attributes;
  time_t Time_Created;
  time_t Time_Modified;
  char* FileName;
} enter;

int FileSize(FILE *fin);
FILE* compress(FILE *fin);
void extract(FILE *fin);

int main(int argc, char* argv[]){
  FILE *fin, *fout, *temp, *temp2;
  int i, j;
  char c;
  int si;
  //Работа с консолью
  if ((argc > 1) && (argc < 4)){
    printf("Error. Wrong Number Of Arguments");
    return 0;
  }
  if (argc == 1){
    printf("\n\nHello, bro. It's archivator. Developers: \n");
    printf("    Gusarov.V, Shchurov Al., Karandaev T.\n\n==================\n\n");
    printf("Look scheme comands:");
    printf("\n  [FileName] [ArchiveName] -a    |Archive");
    printf("\n  [FileName] [ArchiveName] -e    |Extract\n\n==================\n");
    return 0;
  }
    i = 1;
    int* lengths = malloc(sizeof(int) * (argc - 2));
    fout = fopen("temp.txt", "wb");
    while (i < argc - 2){
      fin = fopen(argv[i], "rb");
      while (fscanf(fin, "%c", &c) != EOF){
        lengths[i]++;
        fprintf(fout, "%c", c);
      }
      i++;
    }

  if (strcmp(argv[argc - 1], "-a") == 0){
    if (fopen("temp.txt", "rb") != NULL){
      fin = fopen("temp.txt", "rb");
      //temp = compress(fin);
    } else
        printf("\nCannot find file: %s ", argv[1]);
  } else if (strcmp(argv[argc - 1], "-e") == 0){
      if (fopen(argv[argc - 2], "rb") != NULL){
        fin = fopen(argv[argc - 2], "rb");
        extract(fin);
        return 0;
      } else
          printf("\nCannot find file: %s ", argv[2]);
    } else
        printf("\nUnknow Key, Try again: ");
  enter* Files;
  Files = malloc(sizeof(enter) * (argc-2));
  fout = fopen(argv[argc-2], "wb");
  fprintf(fout, "UPA file archive\nsign: UPA\nHUFF\n1\n");
  fprintf(fout, "%d\n", argc-2);
  for(i = 0; i < argc-3; i++){
    Files[i].FileLength = strlen(argv[i+1]);
    Files[i].FileName = malloc(sizeof(char) * Files[i].FileLength);
    Files[i].FileName = argv[i+1];
    Files[i].SizeOriginal = lengths[i];
    Files[i].Time_Modified = __TIME__;
    Files[i].Attributes = GetFileAttributes(argv[i+1]);
    Files[i].Attributes ^= FILE_ATTRIBUTE_HIDDEN;
    Files[i].Time_Created = 0;
    Files[i].SizePached = FileSize(fout);
    printf("%d", Files[i].SizePached);
  }

  return 0;
}

FILE* compress(FILE *fin){
  FILE* temp;
  temp = compress_huffman(fin, "1", 1);
  return temp;
}

void extract(FILE *fin){
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
  if(strcmp(alg_id, "HUFF") == 0){
    for(i = 0; i < FileCount; i++){}
      //extract_huffman(fin, Files[i].SizePached);
  }
}

int FileSize(FILE *fin){
  int res;
  while (fscanf(fin, "%c") != EOF)
    res++;
  return res;
}

