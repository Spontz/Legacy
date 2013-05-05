#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "set.h"
#include "net2sets.h"

//----------------------------------------
// 
// 
// 


int main(int argc, char **argv)
{
  int i;
  int j;
  SocketSet s;

  initSocketSet(&s, 100);

  for (i = 0; i < 1000; i++)
  {
    addSocketSet(&s, &i);
  }

  for (i = 0; i < 1000; i += 2)
  {
    delSocketSet(&s, &i);
  }

  for (j = firstSocketSet(&s, &i); 
       j != -1;
       j = nextSocketSet(&s, &i))
  {
    printf("item = %d\n", i);
  }

  finitSocketSet(&s);

  return 0;
}
