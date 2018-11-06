/*
File: tree_interface.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "tree_interface.h"

void splitterMerger(int arr[12], int range[2], int depth){
  pid_t left, right;
  int left_end, right_start, status;
  char right_pipe[sizeof(pid_t) + 1], left_pipe[sizeof(pid_t) + 1];

  if (depth == 1)
  {
    //When we reach a depth of 1 we need to call searching nodes
    printf("LEFT LEAF\n");
    printf("RIGHT LEAF\n");
    exit(0);
  }
  else
  {
    //If depth is not 1 we have to continue creating new splitter/merger nodes

    //Finding the new range
    skew(range[0], range[1], 0, &left_end, &right_start);

    //Creating the pipe to communicate with the first child
    sprintf(right_pipe, "R%d", getpid());
    if (mkfifo(right_pipe, 0666) == -1)
    {
      if (errno != EEXIST)
      {
        perror("Right FIFO");
        exit(6);
      }
    }

    //Handling the first child
    right = fork();
    if (right < 0)
    {
      perror("Right Fork Failed");
      exit(1);
    }

    if (right == 0)
    {
      //We update the range and the depth before calling another node
      range[0] = right_start;
      splitterMerger(arr, range, depth - 1);
      exit(0);
    }

    //Before we return to the parent, we create the second child and its pipe
    sprintf(left_pipe, "L%d", getpid());
    if (mkfifo(left_pipe, 0666) == -1)
    {
      if (errno != EEXIST)
      {
        perror("Left FIFO");
        exit(6);
      }
    }

    left = fork();
    if (left < 0)
    {
      perror("Left Fork Failed");
      exit(1);
    }

    if (left == 0)
    {
      range[1] = left_end;
      splitterMerger(arr, range, depth - 1);
      exit(0);
    }

    //Waiting for both of the children to finish
    wait(&status);
    wait(&status);
    remove(left_pipe);
    remove(right_pipe);
    exit(0);
  }
}

void skew(int start, int end, int flag, int *left_end, int *right_start){
  //pairnw arithmo node, afairw 2^h -1 kai briskw to i
  if (flag == 0)
  {
    //Distribute the entries evenly
    *left_end = ((start + end) / 2);
    *right_start = ((start + end) / 2) + 1;
  }
  else
  {
    //Skew the number of entries

  }
}
