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
#include <fcntl.h>
#include <string.h>
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

//Call it as: name of file, pattern, current height, skew, start, end, original height, pipe
int main(int argc, char **argv){
  pid_t left, right;
  int left_end, right_start, status, depth = atoi(argv[3]), rightfd, leftfd, parentfd, check;
  char right_pipe[150], left_pipe[150], endStr[150], startStr[150], depthStr[150], type[1];
  char rl_pipe[150], ll_pipe[150];
  record temp;

  //Finding the new range
  //skew(atoi(argv[5]), atoi(argv[6]), atoi(argv[4]), &left_end, &right_start);
  left_end = atoi(argv[6]); //to fix
  right_start = atoi(argv[5]);  //to fix

  if (depth == 1)
  {
    //When we reach a depth of 1 we need to call two leaf nodes

    //Creating the pipe to communicate with the right leaf
    sprintf(rl_pipe, "RL%d", getpid());
    if (mkfifo(rl_pipe, 0666) == -1)
    {
      if (errno != EEXIST)
      {
        perror("Right FIFO");
        exit(6);
      }
    }

    right = fork();
    if (right < 0)
    {
      perror("Right Searcher Fork Failed");
      exit(1);
    }

    if (right == 0)
    {
      snprintf(startStr, sizeof(int), "%d", right_start);

      execl("leaf", "leaf", argv[1], startStr, argv[6], argv[2], rl_pipe, NULL);
      exit(0);
    }

    //Before we return to the parent, we create the second leaf and its pipe
    sprintf(ll_pipe, "LL%d", getpid());
    if (mkfifo(ll_pipe, 0666) == -1)
    {
      if (errno != EEXIST)
      {
        perror("Left FIFO");
        exit(6);
      }
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

      execl("leaf", "leaf", argv[1], argv[5], endStr, argv[2], ll_pipe, NULL);
      exit(0);
    }

    //Now the parent will receive all the results from the leaf nodes
    parentfd = open(argv[8], O_WRONLY);

    //Read the results from the right leaf
    rightfd = open(rl_pipe, O_RDONLY);
    while (1)
    {
      read(rightfd, type, 1);

      //The children send T when they are about to send a time struct
      //and R when they are about to send a record struct
      if (strcmp(type, "R") == 0)
      {
        read(rightfd, &temp, sizeof(record));

        //Write the results to the pipe the parent opened for us
        write(parentfd, type, 1);
        write(parentfd, &temp, sizeof(record));
      }
      else
      {
        //After reading the times we no longer need to read data
        write(parentfd, type, 1);
        break;
      }
    }

    //Read the results from the left leaf
    leftfd = open(ll_pipe, O_RDONLY);
    while (1)
    {
      read(leftfd, type, 1);

      if (strcmp(type, "R") == 0)
      {
        read(leftfd, &temp, sizeof(record));

        //Write the results to the pipe the parent opened for us
        write(parentfd, type, 1);
        write(parentfd, &temp, sizeof(record));
      }
      else
      {
        //After reading the times we no longer need to read data
        write(parentfd, type, 1);
        break;
      }
    }

    //Waiting for both of the children to finish
    wait(&status);
    wait(&status);
    close(leftfd);
    remove(ll_pipe);
    close(rightfd);
    remove(rl_pipe);
    close(parentfd);
    exit(0);
  }
  else
  {
    //If depth is not 1 we have to continue creating new splitter/merger nodes
    depth--;
    snprintf(depthStr, sizeof(int), "%d", depth);

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
      snprintf(startStr, sizeof(int), "%d", right_start);

      execl("splitterMerger", "splitterMerger", argv[1], argv[2], depthStr, argv[4], startStr, argv[6], argv[7], right_pipe, NULL);
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
      snprintf(endStr, sizeof(int), "%d", left_end);

      execl("splitterMerger", "splitterMerger", argv[1], argv[2], depthStr, argv[4], argv[5], endStr, argv[7], left_pipe, NULL);
      exit(0);
    }

    //Now the parent can receive all the data from its children and send them to the next level
    parentfd = open(argv[8], O_WRONLY);

    //Read the results from the right child
    rightfd = open(right_pipe, O_RDONLY);
    while (1)
    {
      read(rightfd, type, 1);

      //The children send T when they are about to send a time struct
      //and R when they are about to send a record struct
      if (strcmp(type, "R") == 0)
      {
        read(rightfd, &temp, sizeof(record));

        //Write the results to the pipe the parent opened for us
        write(parentfd, type, 1);
        write(parentfd, &temp, sizeof(record));
      }
      else
      {
        //After reading the times we no longer need to read data
        write(parentfd, type, 1);
        break;
      }
    }

    //Read the results from the left child
    leftfd = open(left_pipe, O_RDONLY);
    while (1)
    {
      read(leftfd, type, 1);

      if (strcmp(type, "R") == 0)
      {
        read(leftfd, &temp, sizeof(record));

        //Write the results to the pipe the parent opened for us
        write(parentfd, type, 1);
        write(parentfd, &temp, sizeof(record));
      }
      else
      {
        //After reading the times we no longer need to read data
        write(parentfd, type, 1);
        break;
      }
    }

    //Waiting for both of the children to finish
    wait(&status);
    wait(&status);
    close(leftfd);
    remove(left_pipe);
    close(rightfd);
    remove(right_pipe);
    close(parentfd);
    exit(0);
  }

  return 0;
}
