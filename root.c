/*
File: root.c
Here I implement the root node.
The root node orchestrates the whole program search.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mytypes.h"

//To call it use: name of input file, pattern, height, skew or not
int main(int argc, char **argv){
  pid_t tree;
  int status, recordsCount, treefd;
  char *treeFifo = "druid";
  char end[150], type[1];
  long lSize;
  FILE *ptr, *output;
  record temp;

  //Creating the pipe to communicate with the tree
  if (mkfifo(treeFifo, 0666) == -1)
  {
    if (errno != EEXIST)
    {
      perror("Tree FIFO");
      exit(6);
    }
  }

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

    //Calling the initial binary tree node
    execl("splitterMerger", "splitterMerger", argv[1], argv[2], argv[3], argv[4], "0", end, argv[3], treeFifo, NULL);

    perror("Tree failed to exec");
    exit(1);
  }

  output = fopen("output.txt", "w");
  if (output == NULL)
  {
    perror("Couldn't create output file");
    exit(-1);
  }

  //Read the results from the splitterMerger nodes
  treefd = open(treeFifo, O_RDONLY);
  while (1)
  {
    read(treefd, type, 1);

    //The children send T when they are about to send a time struct
    //and R when they are about to send a record struct
    if (strcmp(type, "R") == 0)
    {
      read(treefd, &temp, sizeof(record));

      //Write the results to an ASCII file to be able to give them to the sort node
      //printf("%lu %s %s ", temp.custid, temp.FirstName, temp.LastName);
      //printf("%s %d %s ", temp.Street, temp.HouseID, temp.City);
      //printf("%s %f\n", temp.postcode, temp.amount);
    }
    else
    {
      //After reading the times we no longer need to read data
      break;
    }
  }


  //Waiting for the binary tree to finish searching and sending the results
  wait(&status);
  fclose(output);
  close(treefd);
  remove(treeFifo);

  //Now we can call the sort() function to sort
  //the results the tree produced

  return 0;
}
