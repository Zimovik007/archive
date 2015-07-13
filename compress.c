#include <stdio.h>
#include <stdlib.h>

int frequency[256];

static void count_frequency(FILE *fin)
{
	char c;
	while (fscanf(fin, "%c", &c)) ++frequency[c];	
}

