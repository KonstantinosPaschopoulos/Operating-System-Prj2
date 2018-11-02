/*
File: myfind.c
Here is the main of my app.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "tree_interface.h"

int main(int argc, char **argv){
  int arr[12] = {1, 3, 4, 654, 234, 543, 34, 54, 34, 45, 34, 34};
  int range[2];
  range[0] = 0;
  range[1] = 11;

  printf("I am %d\n", (int)getpid());

  splitterMerger(arr, range, 4);

  return 0;
}
