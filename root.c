/*
File: root.c
Here I implement the root node.
The root node orchestrates the whole program search.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "mytypes.h"

//To call it use: name of input file, pattern, height, skew or not
int main(int argc, char **argv){
  pid_t tree;
  int status, recordsCount;
  char end[150];
  long lSize;
  FILE *ptr;

  tree = fork();
  if (tree < 0)
  {
    perror("Tree Fork Failed");
    exit(1);
  }

  if (tree == 0)
  {
    //Open the input file and count how many records it contains
    ptr = fopen(argv[1], "rb");
    if (ptr == NULL)
    {
      perror("Could not open input binary file");
      exit(5);
    }

    fseek(ptr, 0, SEEK_END);
    lSize = ftell(ptr);

    recordsCount = lSize / sizeof(record);

    rewind(ptr);
    fclose(ptr);

    //After finding out how many records it contains I have to
    //transform that number into a string in order to call the binary tree
    snprintf(end, sizeof(int), "%d", recordsCount);

    //Calling the binary tree with the correct arguments
    execl("splitterMerger", "splitterMerger", argv[1], argv[2], argv[3], argv[4], "0", end, argv[3], NULL);

    perror("Tree failed to exec");
    exit(1);
  }

  //Waiting for the binary tree to finish searching
  wait(&status);

  //Now we can call the sort() function to sort
  //the results the tree produced

  return 0;
}
