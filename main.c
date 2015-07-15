#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <windows.h>
#include "compress.h"
#include "extract_huff.h"

typedef struct entries{
  unsigned int FileLength;
  unsigned int SizePacked;
  unsigned int SizeOriginal;
  unsigned int Attributes;
  unsigned int Time_Created;
  unsigned int Time_Modified;
  char* FileName;
  FILE* compress_file;
} enter;

FILE * compress(FILE *fin, unsigned int *FileSizeOriginal, unsigned int *FileSizePacked);
void extract(FILE *fin);
void archivator(char* argv[], int argc); //Подготавливает данные к архивации и архивирует, если нашелся ключ
void extractor(); //Подготавливает данные к деархивации и деархивирует, если нашелся ключ

char* ArchiveName;
char** FileNames;
FILE *fout;
enter* Files;

int main(int argc, char* argv[]){
	
	FILE *fi, *fu, *or, *ar;
	fi = fopen("test5.in", "rb");
	fu = fopen("test5.iin", "wb");
	unsigned int ors = 0, ars = 0;
	ar = compress_huffman(fi, &ors, &ars);
	rewind(ar);
	or = extract_huffman(ar, ors);
	char tc;
	rewind(or);
	while (!feof(or))
	{
		if (fscanf(or, "%c", &tc) <= 0) break;
		fprintf(fu, "%c", tc);
	}
	fclose(fi);
	fclose(fu);
	return 0;

  FILE *temp;
  int i = 1;
  char c = (int)0;
  char test1[17];
  FileNames = malloc(sizeof(char));
  //Работа с консолью
  if (argc == 1){
    printf("\n\nHello, bro. It's archivator. Developers: \n");
    printf("    Gusarov.V, Shchurov Al., Karandaev T.\n\n==================\n\n");
    printf("Look scheme comands:");
    printf("\n  [FileName...] [ArchiveFile] -a -solid(-nosolid)    |Archive");
    printf("\n  [ArchiveName] [FileName] -e                        |Extract\n\n==================\n");
    return 0;
  }

  //Поиск -а -е в параметрах
  for(i = 1; i < argc; i++){
    if (strcmp(argv[i], "-a") == 0){
      archivator(argv, argc);
      c = 1;
      break;
    }
    if (strcmp(argv[i], "-e") == 0){
      extractor();
      c = 1;
      break;
    }
  }
  //Если отсутствует ключ то мы ищем сигнатуру в первом файле
  if (c == 0){
    temp = fopen(argv[1], "rb");
    fgets(test1, 17, temp);
    fclose(temp);
    if (strcmp(test1, "UPA file archive") == 0)extractor();
    else archivator(argv, argc);
  }
  return 0;
}

FILE * compress(FILE *fin, unsigned int *FileSizeOriginal, unsigned int *FileSizePacked){
  FILE* temp;
  temp = compress_huffman(fin, FileSizeOriginal, FileSizePacked);
  return temp;
}

void extract(FILE *fin){
  int i, d, FileCount;
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
    fscanf(fin, "%s\n", Files[i].FileName);
    fscanf(fin, "%d\n", &Files[i].SizePacked);
    fscanf(fin, "%d\n", &Files[i].SizeOriginal);
    fscanf(fin, "%d\n", &Files[i].Attributes);
    fscanf(fin, "%d\n", &Files[i].Time_Created);
    fscanf(fin, "%d\n", &Files[i].Time_Modified);
  }
  if(strcmp(alg_id, "HUFF") == 0){
    for(i = 0; i < FileCount; i++){
      //extract_huffman(fin, Files[i].SizePached);
    }
  }
}

void archivator(char* argv[], int argc){
  int i = 1, count;
  unsigned int FileSizePacked, FileSizeOriginal;
  char c = (int)1;
  FILE *temp, *fbuf;
  //<===========БЛОК ДО FAT entries
  //Считали входные данные и выходные
  while((argv[i][0] != '-') || (i == argc)){
    if (argv[i+1][0] == '-'){
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
  count = i - 1;
  Files = malloc(sizeof(enter) * count);
  //Начинаем оформлять выходной файл
  fout = fopen(ArchiveName, "wb");
  fprintf(fout, "UPA file archive\nsign: UPA\nHUFF\n");
  //Ищем IsSolid
  for(i = i; i < argc; i++){
    if (strcmp(argv[i], "-solid") == 0){c = 1; break;}
    if (strcmp(argv[i], "-nosolid") == 0){c = 0; break;}
  }
  //Выводи IsSolid и количсетво файлов
  fprintf(fout, "%d\n", (int)c);
  fprintf(fout, "%d\n", count);
  //<============FAT ENTRIES:
    //ПОДГОТОВКА информации
  fbuf = fopen("temp.txt", "wb");
  for(i = 0; i < count; i++){
    temp = fopen(argv[i+1], "rb");
    Files[i].FileLength = strlen(argv[i+1]);
    Files[i].FileName = argv[i+1];
    Files[i].Attributes = 0;
    Files[i].Time_Created = 0;//__TIME__;
    Files[i].Time_Modified = 0;//__TIME__;
    Files[i].SizePacked = 0;

    if (c == 1){
      char tc;
      while(!feof(temp)){
		fscanf(temp, "%c", &tc);
        fprintf(fbuf, "%c", tc);
      }
    } else {
        Files[i].compress_file = compress(temp, &FileSizeOriginal, &FileSizePacked);
        Files[i].SizePacked = FileSizePacked;
        FileSizePacked = 0;
      }

    fseek(temp, 0, SEEK_END);
    Files[i].SizeOriginal = ftell(temp);
    fclose(temp);
  }
  rewind(fbuf);
  if (c == 1){
    Files[0].compress_file = compress(fbuf, &FileSizeOriginal, &FileSizePacked);
    Files[0].SizePacked = FileSizePacked;
    FileSizePacked = 0;
  }
  //Запись в файл
  for(i = 0; i < count; i++){
    fprintf(fout, "%d\n", Files[i].FileLength);
    fprintf(fout, "%s\n", Files[i].FileName);
    fprintf(fout, "%d\n", Files[i].SizePacked);
    fprintf(fout, "%d\n", Files[i].SizeOriginal);
    fprintf(fout, "%d\n", Files[i].Attributes);
    fprintf(fout, "%d\n", Files[i].Time_Created);
    fprintf(fout, "%d\n", Files[i].Time_Modified);
  }
  //<==== Сжатые данные
  if (c == 1){
    rewind(Files[0].compress_file);
    while(!feof(Files[0].compress_file)){
	  char tc;
	  fscanf(Files[0].compress_file, "%c", &tc);
      fprintf(fout, "%c", tc);
    }
  } else{
      for(i = 0; i < count; i++){
        rewind(Files[i].compress_file);
        while(!feof(Files[i].compress_file)){
		  char tc;
		  fscanf(Files[i].compress_file, "%c", &tc);
          fprintf(fout, "%c", tc);
        }
      }
    }
}

void extractor(){

}
