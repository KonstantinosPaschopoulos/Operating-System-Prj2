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
#include <sys/time.h>
#include <signal.h>
#include "mytypes.h"
#include <math.h>

volatile sig_atomic_t num_signals = 0;

void sig_handler(int signo)
{
  signal(SIGUSR2, sig_handler);

  num_signals++;
}

//To call it use: name of input file, pattern, height, skew or not
int main(int argc, char **argv){
  pid_t tree, sort_pid;
  int status, recordsCount, treefd, type;
  char *treeFifo = "druid";
  char end[150], pid[10], endRange[150];
  long lSize;
  FILE *ptr, *output;
  record temp;
  timesSM SM_times;

  if (signal(SIGUSR2, sig_handler) == SIG_ERR)
  {
    perror("Could not create signal handler");
    exit(10);
  }

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

    recordsCount = (int) lSize / sizeof(record);

    rewind(ptr);
    fclose(ptr);

    //After finding out how many records it contains I have to
    //transform that number into a string in order to call the binary tree
    sprintf(end, "%d", recordsCount);

    sprintf(endRange,"%d", (int)pow(2, atoi(argv[3])));
    sprintf(pid, "%d", (int)getppid());

    //Calling the initial binary tree node
    execl("splitterMerger", "splitterMerger", argv[1], argv[2], argv[3], argv[4], "0", end, endRange, treeFifo, pid, "1", endRange, NULL);

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
    read(treefd, &type, sizeof(int));

    //The children send 0 when they are about to send a time struct
    //and 1 when they are about to send a record struct
    if (type == 1)
    {
      read(treefd, &temp, sizeof(record));

      //Write the results to an ASCII file to be able to give them to the sort node
      fprintf(output, "%lu %s %s ", temp.custid, temp.LastName, temp.FirstName);
      fprintf(output, "%s %d %s ", temp.Street, temp.HouseID, temp.City);
      fprintf(output, "%s %f\n", temp.postcode, temp.amount);
    }
    else if (type == 0)
    {
      //Read all the times
      read(treefd, &SM_times, sizeof(timesSM));
      printf("Max Time of Searcher nodes %f\n", SM_times.maxS);
      printf("Min Time of Searcher nodes %f\n", SM_times.minS);
      printf("Average Time of Searcher nodes %f\n", SM_times.avgS);
      printf("Max Time of splitter/merger nodes %f\n", SM_times.maxSM);
      printf("Min Time of splitter/merger nodes %f\n", SM_times.minSM);
      printf("Average Time of splitter/merger nodes %f\n", SM_times.avgSM);
    }
    else if (type == -1)
    {
      break;
    }
  }


  //Waiting for the binary tree to finish searching and sending the results
  wait(&status);
  fclose(output);
  close(treefd);
  remove(treeFifo);

  printf("Root received %d SIGUSR2 signals\n", num_signals);

  //Now we can call the sort() function to sort
  //the results the tree produced
  sort_pid = fork();
  if (sort_pid < 0)
  {
    perror("Sort Fork Failed");
    exit(1);
  }
  if (sort_pid == 0)
  {
    execlp("sort", "sort", "output.txt", "-g", "-k", "1", NULL);

    perror("Sort failed to exec");
    exit(1);
  }

  wait(&status);
  remove("output.txt");

  return 0;
}
