#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../Archive/compress.h"
#include "../Archive/extract_huff.h"
#include "../Archive/extract_nope.h"
#include "../Archive/comp_nope.h"
//#include "compress_lzw.h"
//#include "extract_lzw.h"

typedef struct entry1{
  unsigned int FileLength;
  unsigned int SizePacked;
  unsigned int SizeOriginal;
  unsigned int Attributes;
  unsigned int Time_Created;
  unsigned int Time_Modified;
  char*        FileName;
  FILE*        compress_file;
} entry;

void  Archive                  (char* argv[], int argc);
void  Extract                  ();
int   get_archive_file_names   (char* argv[], int argc);
char  print_solid_count        (char* argv[], int argc, int count);
void  print_FAT                (int count);
void  prepare_files            (char c, int count, char* argv[]);
void  print_compress_data      (char c, int count);
char* read_signature_files     (FILE *fin, int *FileCoint, int *BufLen, int *d);
void  extract                  (char* alg_id, int d, FILE *fin, int FileCount, int BufLen);

char*  ArchiveName;
char** FileNames;
FILE*  fout;
entry* Files;

int main(int argc, char* argv[]){

  FILE *temp;
  int  i = 1;
  char c = (int)0;
  char test1[17];
  FileNames = malloc(sizeof(char));


  if (argc == 1){
    printf("\n\nHello, bro. It's archivator. Developers: \n");
    printf("    Gusarov.V, Shchurov Al., Karandaev T.\n\n==================\n\n");
    printf("Usage:");
    printf("\n  [FileName...] [ArchiveFile] -a -nosolid(-solid)    |Archive");
    printf("\n  [ArchiveName] [FileName] -e                        |Extract\n\n==================\n");
    return 0;
  }

  //Поиск -а -е в параметрах
  for (i = 1; i < argc; i++){
    if (strcmp(argv[i], "-a") == 0){
      Archive(argv, argc);
      c = 1;
      break;
    }
    if (strcmp(argv[i], "-e") == 0){
      ArchiveName = malloc(sizeof(char) * strlen(argv[1]));
      ArchiveName = argv[1];
      Extract();
      c = 1;
      break;
    }
  }

  //Если отсутствует ключ то мы ищем сигнатуру в первом файле
  if (c == 0){
    temp = fopen(argv[1], "rb");
    fgets(test1, 17, temp);
    fclose(temp);
    if (strcmp(test1, "UPA file archive") == 0){
      ArchiveName = argv[1];
      Extract();
    } else Archive(argv, argc);
  }
  return 0;
}

//====================================
//====================================
//====================================

void Archive(char* argv[], int argc){
  int count;
  char c = (int)1;

  time_t t = time(NULL);
  struct tm* ctme = localtime(&t);
  printf("%02d:%02d:%02d compressing...\n", ctme->tm_hour, ctme->tm_min, ctme->tm_sec);

  count = get_archive_file_names(argv, argc);
  c = print_solid_count(argv, argc, count);
  Files = malloc(sizeof(entry) * count);
  prepare_files(c, count, argv);
  print_FAT(count);
  print_compress_data(c, count);

  t = time(NULL);
  ctme = localtime(&t);
  printf("%02d:%02d:%02d complete\n", ctme->tm_hour, ctme->tm_min, ctme->tm_sec);
}

int get_archive_file_names(char* argv[], int argc){
  int i = 1;
  while ((i < argc) && (argv[i][0] != '-')){
    if ((i+1 == argc) || (argv[i+1][0] == '-')){
      ArchiveName = malloc(sizeof(char) * strlen(argv[i]));
      ArchiveName = argv[i];
      break;
    }
    else{
      FileNames = realloc(FileNames, sizeof(char) * i);
      FileNames[i-1] = malloc(sizeof(char) * strlen(argv[i]));
      FileNames[i-1] = argv[i];
    }
    i++;
  }
  return i - 1;
}

char print_solid_count(char* argv[], int argc, int count){
  int i;
  char c = (int)1;
  fout = fopen(ArchiveName, "wb");
  fprintf(fout, "UPA file archive\nsign: UPA\nHUFF\n");
  for (i = count+1; i < argc; i++){
    if (strcmp(argv[i], "-solid") == 0){c = 1; break;}
    if (strcmp(argv[i], "-nosolid") == 0){c = 0; break;}
  }
  fprintf(fout, "%d\n", (int)c);
  fprintf(fout, "%d\n", count);
  return c;
}

void prepare_files(char c, int count, char* argv[]){
  int i;
  unsigned int FileSizePacked = 0, FileSizeOriginal = 0;
  FILE *fbuf, *temp;
  fbuf = fopen("temp.txt", "wb");
  for (i = 0; i < count; i++){
    temp = fopen(argv[i+1], "rb");
    Files[i].FileLength = strlen(argv[i+1]);
    Files[i].FileName = argv[i+1];
    Files[i].Attributes = 0;
    Files[i].Time_Created = 0;//__TIME__;
    Files[i].Time_Modified = 0;//__TIME__;
    Files[i].SizePacked = 0;
    Files[i].compress_file = compress_huffman(temp, &FileSizeOriginal, &FileSizePacked);
    Files[i].SizePacked = FileSizePacked;
    Files[i].SizeOriginal = FileSizeOriginal;
    FileSizePacked = 0;
    FileSizeOriginal = 0;

    if (c == 1){
      char tc;
      rewind(temp);
      while(!feof(temp)){
		if (fscanf(temp, "%c", &tc) != EOF)
          fprintf(fbuf, "%c", tc);
      }
    }

    fclose(temp);
  }
  fclose(fbuf);
  if (c == 1){
    fbuf = fopen("temp.txt", "rb");
    Files[0].compress_file = compress_huffman(fbuf, &FileSizeOriginal, &FileSizePacked);
    Files[0].SizePacked = FileSizePacked;
    FileSizePacked = 0;
    FileSizeOriginal = 0;
  }
}

void print_FAT(int count){
  int i;
  for(i = 0; i < count; i++){
    fprintf(fout, "%d\n", Files[i].FileLength);
    fprintf(fout, "%s\n", Files[i].FileName);
    fprintf(fout, "%d\n", Files[i].SizePacked);
    fprintf(fout, "%d\n", Files[i].SizeOriginal);
    fprintf(fout, "%d\n", Files[i].Attributes);
    fprintf(fout, "%d\n", Files[i].Time_Created);
    fprintf(fout, "%d\n", Files[i].Time_Modified);
  }
}

void print_compress_data(char c, int count){
  int i;
  if (c == 1){
    rewind(Files[0].compress_file);
    printf("2\n");
    while (!feof(Files[0].compress_file)){
      printf("3\n");
	  char tc;
	  if (fscanf(Files[0].compress_file, "%c", &tc) != EOF)
        fprintf(fout, "%c", tc);
    }
  } else{
      for (i = 0; i < count; i++){
        rewind(Files[i].compress_file);
        while (!feof(Files[i].compress_file)){
		  char tc;
		  if (fscanf(Files[i].compress_file, "%c", &tc) != EOF)
            fprintf(fout, "%c", tc);
        }
      }
    }
}

//======================================
//======================================
//======================================

void Extract(){
  int   d = 0 , FileCount = 0, BufLen = 0;
  char* alg_id;
  FILE  *fin;

  fin = fopen(ArchiveName, "rb");
  alg_id = malloc(sizeof(char) * 5);

  time_t t = time(NULL);
  struct tm* ctme = localtime(&t);
  printf("%02d:%02d:%02d extracting...\n", ctme->tm_hour, ctme->tm_min, ctme->tm_sec);

  alg_id = read_signature_files(fin, &FileCount, &BufLen, &d);
  extract(alg_id, d, fin, FileCount, BufLen);

  t = time(NULL);
  ctme = localtime(&t);
  printf("%02d:%02d:%02d complete\n", ctme->tm_hour, ctme->tm_min, ctme->tm_sec);
}

char* read_signature_files(FILE *fin, int* FileCount, int* BufLen, int* d){
  int   i;
  char  test1[17], test2[10];
  char* alg_id;
  alg_id = malloc(sizeof(char) * 5);
  fgets(test1, 17, fin); fscanf(fin, "\n");
  fgets(test2, 10, fin); fscanf(fin, "\n");
  fgets(alg_id, 5, fin); fscanf(fin, "\n");
  fscanf(fin, "%d", d);
  if (strcmp(test1, "UPA file archive") != 0){printf("Wrong Format File!\n"); exit(0);}
  if (strcmp(test2, "sign: UPA") != 0){printf("Wrong Format File!\n"); exit(0);}
  fscanf(fin,"%d\n", FileCount);
  Files = malloc(sizeof(entry) * (*FileCount));
  for(i = 0; i < *FileCount; i++){
    fscanf(fin, "%d\n", &Files[i].FileLength);
    Files[i].FileName = malloc(sizeof(char) * Files[i].FileLength);
    fscanf(fin, "%s", Files[i].FileName);
    fscanf(fin, "%d\n", &Files[i].SizePacked);
    fscanf(fin, "%d\n", &Files[i].SizeOriginal);
    fscanf(fin, "%d\n", &Files[i].Attributes);
    fscanf(fin, "%d\n", &Files[i].Time_Created);
    fscanf(fin, "%d\n", &Files[i].Time_Modified);
    *BufLen += Files[i].SizeOriginal;
  }
  return alg_id;
}

void extract(char* alg_id, int d, FILE *fin, int FileCount, int BufLen){
  int  i, j;
  char c;
  FILE *temp, *temp2;
  if (strcmp(alg_id, "HUFF") == 0){
    if (d == 0){
      for (i = 0; i < FileCount; i++){
        temp = extract_huffman(fin, Files[i].SizeOriginal);
        temp2 = fopen(Files[i].FileName, "wb");
        rewind(temp);
        while (!feof(temp)){
          if (fscanf(temp, "%c", &c) != EOF)
            fprintf(temp2, "%c", c);
        }
        fclose(temp2);
        fclose(temp);
      }
    } else {
        temp = extract_huffman(fin, BufLen);
        rewind(temp);
        for (i = 0; i < FileCount; i++){
          temp2 = fopen(Files[i].FileName, "wb");
          for (j = 0; j < Files[i].SizeOriginal; j++){
            fscanf(temp, "%c", &c);
            fprintf(temp2, "%c", c);
          }
          fclose(temp2);
        }
        fclose(temp);
      }
  }
  return;
}
