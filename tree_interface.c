/*
File: tree_interface.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "tree_interface.h"

/*
void splitterMerger(int arr[12], int range[2], int depth){
  pid_t child;
  int status;

  printf("DEPTH %d\n", depth);

  if (depth == 1)
  {
    //When we reach a depth of 1 we need to call the searching nodes
    return;
  }
  else
  {
    //If depth is not 1 we have to continue creating new splitter/merger nodes

    child = fork();
    if (child < 0)
    {
      perror("Fork Failed");
      exit(1);
    }

    if (child == 0)
    {
      //Creating the children first
      splitterMerger(arr, range, depth - 1);
      splitterMerger(arr, range, depth - 1);

      exit(0);
    }
    else
    {
      //The parent waits for the children to finish before merging
      if (waitpid(child, &status, 0) == child)
      {
        //merging
      }
      else
      {
        perror("Error with child");
      }
    }
  }
}
*/



void splitterMerger(int arr[12], int range[2], int depth){
  pid_t left, right;

  if (depth == 1)
  {
    //When we reach a depth of 1 we need to call searching nodes
    exit(0);
  }
  else
  {
    //If depth is not 1 we have to continue creating new splitter/merger nodes

    right = fork();
    if (right < 0)
    {
      perror("Right Fork Failed");
      exit(1);
    }
    if (right == 0)
    {
      //Handling the first child
      splitterMerger(arr, range, depth - 1);
      exit(0);
    }

    //Before we return to the parent, we create the second child
    left = fork();
    if (left < 0)
    {
      perror("Left Fork Failed");
      exit(1);
    }
    if (left == 0)
    {
      splitterMerger(arr, range, depth - 1);
      exit(0);
    }

    wait(NULL);
  }
}

int skew(int start, int end, int flag){
  //pairnw arithmo node, afairw 2^h -1 kai briskw to i
  if (flag == 0)
  {
    //*start = *end / 2;
    //distribute the entries evenly
    return ((start + end) / 2);
  }
  else
  {
    //skew the number of entries

  }
}
