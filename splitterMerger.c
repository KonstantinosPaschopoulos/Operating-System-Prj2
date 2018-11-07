/*
File: splitterMerger
This file contains the code the splitter/merger nodes use.
Firstly depending on the given height they create the necessary internal nodes.
After that they call the leaf executables to do the searching.
Finally they collect all the results and send them to their parent node.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "mytypes.h"

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

//Call it as: name of file, pattern, current height, skew, start, end, original height
int main(int argc, char **argv){
  pid_t left, right;
  int left_end, right_start, status, depth = atoi(argv[3]);
  char right_pipe[150], left_pipe[150], endStr[150], startStr[150], depthStr[150];

  //Finding the new range
  skew(atoi(argv[5]), atoi(argv[6]), atoi(argv[4]), &left_end, &right_start);

  if (depth == 1)
  {
    //When we reach a depth of 1 we need to call two leaf nodes
    right = fork();
    if (right < 0)
    {
      perror("Right Searcher Fork Failed");
      exit(1);
    }

    if (right == 0)
    {
      snprintf(startStr, sizeof(int), "%d", right_start);
      execl("leaf", "leaf", argv[1], startStr, argv[6], argv[2], NULL);
      exit(0);
    }

    //Creating the second leaf node
    left = fork();
    if (left < 0)
    {
      perror("Left Searcher Fork Failed");
      exit(1);
    }

    if (left == 0)
    {
      snprintf(endStr, sizeof(int), "%d", left_end);
      execl("leaf", "leaf", argv[1], argv[5], endStr, argv[2], NULL);
      exit(0);
    }

    //Waiting for both of the children to finish
    wait(&status);
    wait(&status);
    exit(0);
  }
  else
  {
    //If depth is not 1 we have to continue creating new splitter/merger nodes
    depth--;
    snprintf(depthStr, sizeof(int), "%d", depth);

    /*
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
    */

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
      snprintf(startStr, sizeof(int), "%d", right_start);
      execl("splitterMerger", "splitterMerger", argv[1], argv[2], depthStr, argv[4], startStr, argv[6], argv[7], NULL);
      exit(0);
    }

    /*
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
    */

    left = fork();
    if (left < 0)
    {
      perror("Left Fork Failed");
      exit(1);
    }

    if (left == 0)
    {
      snprintf(endStr, sizeof(int), "%d", left_end);
      execl("splitterMerger", "splitterMerger", argv[1], argv[2], depthStr, argv[4], argv[5], endStr, argv[7], NULL);
      exit(0);
    }

    //Waiting for both of the children to finish
    wait(&status);
    wait(&status);
    //remove(left_pipe);
    //remove(right_pipe);
    exit(0);
  }

  return 0;
}
