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




program project1;
uses sysutils;
var
  inp: file of byte;
  outp: text;
  //outp: text;
  count: word;
  buf: byte;
  m: array[0..7] of boolean;
  a: string;
  i, j: integer;

begin
  assign(inp, 'input.txt');
  assign(outp, 'output.txt');
  reset(inp);
  rewrite(outp);

  while not EOF(inp) do
  begin
    BlockRead(inp, buf, 1);
    i := 128;
    while i >= 1 do
    begin
      m[j] := ((buf or i) = buf);
      inc(j);
      i := i div 2;
    end;
    for i := 0 to 7 do
      a += inttostr(ord(m[i]));

    Writeln(outp, a);           //Block
  end;
  //write({ord(m[i])}  a);
  //readln;
  close(inp);
  close(outp);
end.

      
