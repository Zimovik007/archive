#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compress(FILE *fin, char ArchiveName[200]);
void extract(FILE *fin, char FileName[200]);

int main()
{
    FILE *fin;
    printf("Hello, User.\n It's archivator develompment:\n   Gusarov V., Schyrov Al., Karandaev T.\n");
    printf("Let's start: ");
    int i, j;
    char address[200];
    char FileName[200];
    char ArchiveName[200];
    for(i = 0; i < 200; i++){
      FileName[i] = NULL;
      ArchiveName[i] = NULL;
    }
    //Работа с консолью
    while(1){
      printf("\n             ");
      gets(address);
      if (strcmp(address, "exit") == 0) return 0;
      i = 0;
      while (1){
        if (address[i] == ' ') break;
        FileName[i] = address[i];
        i++;
      }
      i++;
      j = 0;
      while (1){
        if (address[i] == ' ') break;
        ArchiveName[j] = address[i];
        i++; j++;
      }
      i += 2;
      if (address[i] == 'a'){
        if (fopen(FileName, "rb") != NULL){
          fin = fopen(FileName, "rb");
          compress(fin, ArchiveName);
          break;
        } else printf("\nCannot find file: %s ", FileName);
      } else if (address[i] == 'e'){
          if (fopen(ArchiveName, "rb") != NULL){
            fin = fopen(ArchiveName, "rb");
            extract(fin, FileName);
            break;
          } else printf("\nCannot find file: %s ", ArchiveName);
        } else{
            printf("\nUnknowError, Try again: ");
          }
    }

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
  char test[3];
  test[0] = fgetc(fin);
  test[1] = fgetc(fin);
  test[2] = fgetc(fin);
  if (strcmp(test, "vlt") == 0){
    while (1){
      c = fgetc(fin);
      if (c == EOF) break;
      fprintf(fout, "%c", c);
    }
  } else printf("Error... Wrong Format File...");
}

