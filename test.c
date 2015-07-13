#include <stdio.h>
#include <stdlib.h>

int main(){

   // 2 -> 10

  int a[8] = {1,1,1,0,0,1,1,1};
  int k = 0;
  k = (a[0] << 7) | (a[1] << 6) | (a[2] << 5) |
      (a[3] << 4) | (a[4] << 3) | (a[5] << 2) | (a[6] << 1) | (a[7]);
  printf("%d\n", k);

   // 10 -> 2

  int num = 201;
  int m[8];
  int i, j = 0;
  for (i = 128; i >= 1; i /= 2){
    m[j++] = ((num | i) == num);
    printf("%d ", m[j-1]);
  }

  return 0;
}
