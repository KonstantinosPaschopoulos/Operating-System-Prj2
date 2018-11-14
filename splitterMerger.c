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
#include <sys/time.h>
#include <signal.h>
#include "mytypes.h"

void skew(int searchers, int records_num, int searcher_num, int flag, int *start, int *end){
  int sum_total, i, sum, j;

  if (flag == 0)
  {
    //Distribute the entries evenly
    *start = (searcher_num - 1) * (records_num / searchers);

    //Because the division (records_num / searchers) might sometimes leave a remainder
    //the last searcher has to search until the end of the file
    if (searcher_num == searchers)
    {
      *end = records_num;
    }
    else
    {
      *end = searcher_num * (records_num / searchers);
    }
  }
  else
  {
    //Skew the number of entries
    sum_total = 0;
    for (i = 1; i <= (searcher_num - 1); i++)
    {
      sum = 0;
      for (j = 1; j <= searchers; j++)
      {
        sum += j;
      }
      sum_total += (records_num * i) / sum;
    }
    *start = sum_total;

    if (searcher_num == searchers)
    {
      *end = records_num;
    }
    else
    {
      sum = 0;
      for (j = 1; j <= searchers; j++)
      {
        sum += j;
      }
      *end = sum_total + ((records_num * searcher_num) / sum);
    }
  }
}

//Call it as: name of file, pattern, current height, skew, start, end, number of searchers, pipe, root pid, start of rangem end of range
int main(int argc, char **argv){
  pid_t left, right;
  int search_start, search_end, status, depth = atoi(argv[3]), rightfd, leftfd, parentfd, type, tmp;
  char right_pipe[150], left_pipe[150], endStr[150], startStr[150], depthStr[150], startRange[150], endRange[150];
  char rl_pipe[150], ll_pipe[150];
  record temp;
  timesS leaf_times1, leaf_times2;
  timesSM SM_times, sm_1, sm_2;
  struct timeval begin, end;
  double total_t;

  if (depth == 1)
  {
    //When we reach a depth of 1 we need to call two leaf nodes
    gettimeofday(&begin, NULL);

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
      skew(atoi(argv[7]), atoi(argv[6]), atoi(argv[11]), atoi(argv[4]), &search_start, &search_end);
      sprintf(startStr, "%d", search_start);
      sprintf(endStr, "%d", search_end);

      execl("leaf", "leaf", argv[1], startStr, endStr, argv[2], rl_pipe, argv[9], NULL);
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
      skew(atoi(argv[7]), atoi(argv[6]), atoi(argv[10]), atoi(argv[4]), &search_start, &search_end);
      sprintf(startStr, "%d", search_start);
      sprintf(endStr, "%d", search_end);

      execl("leaf", "leaf", argv[1], startStr, endStr, argv[2], ll_pipe, argv[9], NULL);
      exit(0);
    }

    //Now the parent will receive all the results from the leaf nodes
    parentfd = open(argv[8], O_WRONLY);

    //Read the results from the right leaf
    rightfd = open(rl_pipe, O_RDONLY);
    while (1)
    {
      read(rightfd, &type, sizeof(int));

      //The children send 0 when they are about to send a time struct
      //and 1 when they are about to send a record struct
      if (type == 1)
      {
        read(rightfd, &temp, sizeof(record));

        //Write the results to the pipe the parent opened for us
        write(parentfd, &type, sizeof(int));
        write(parentfd, &temp, sizeof(record));
      }
      else if (type == 0)
      {
        read(rightfd, &leaf_times1, sizeof(timesS));
      }
      else if (type == -1)
      {
        break;
      }
    }

    //Read the results from the left leaf
    leftfd = open(ll_pipe, O_RDONLY);
    while (1)
    {
      read(leftfd, &type, sizeof(int));

      if (type == 1)
      {
        read(leftfd, &temp, sizeof(record));

        //Write the results to the pipe the parent opened for us
        write(parentfd, &type, sizeof(int));
        write(parentfd, &temp, sizeof(record));
      }
      else if (type == 0)
      {
        read(leftfd, &leaf_times2, sizeof(timesS));
      }
      else if (type == -1)
      {
        break;
      }
    }

    //Finding the min, max, and average times of the leaf nodes before sending them
    if (leaf_times1.time < leaf_times2.time)
    {
      SM_times.maxS = leaf_times2.time;
      SM_times.minS = leaf_times1.time;
    }
    else
    {
      SM_times.maxS = leaf_times1.time;
      SM_times.minS = leaf_times2.time;
    }

    SM_times.avgS = (leaf_times1.time + leaf_times2.time) / 2;

    gettimeofday(&end, NULL);
    total_t = (double) (end.tv_usec - begin.tv_usec) / 1000000;

    SM_times.maxSM = total_t;
    SM_times.minSM = total_t;
    SM_times.avgSM = total_t;

    type = 0;
    write(parentfd, &type, sizeof(int));
    write(parentfd, &SM_times, sizeof(timesSM));

    //After both searchers return the results we signal the parent to stop reading
    type = -1;
    write(parentfd, &type, sizeof(int));

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
    gettimeofday(&begin, NULL);

    depth--;
    sprintf(depthStr, "%d", depth);

    //Finding the midpoint
    tmp = (int)((atoi(argv[11]) + atoi(argv[10])) / 2);

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
      sprintf(startRange, "%d", (tmp + 1));

      execl("splitterMerger", "splitterMerger", argv[1], argv[2], depthStr, argv[4], argv[5], argv[6], argv[7], right_pipe, argv[9], startRange, argv[11], NULL);
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
      sprintf(endRange, "%d", tmp);

      execl("splitterMerger", "splitterMerger", argv[1], argv[2], depthStr, argv[4], argv[5], argv[6], argv[7], left_pipe, argv[9], argv[10], endRange, NULL);
      exit(0);
    }

    //Now the parent can receive all the data from its children and send them to the next level
    parentfd = open(argv[8], O_WRONLY);

    //Read the results from the right child
    rightfd = open(right_pipe, O_RDONLY);
    while (1)
    {
      read(rightfd, &type, sizeof(int));

      //The children send 0 when they are about to send a time struct
      //and 1 when they are about to send a record struct
      if (type == 1)
      {
        read(rightfd, &temp, sizeof(record));

        //Write the results to the pipe the parent opened for us
        write(parentfd, &type, sizeof(int));
        write(parentfd, &temp, sizeof(record));
      }
      else if (type == 0)
      {
        read(rightfd, &sm_1, sizeof(timesSM));
      }
      else if (type == -1)
      {
        break;
      }
    }

    //Read the results from the left child
    leftfd = open(left_pipe, O_RDONLY);
    while (1)
    {
      read(leftfd, &type, sizeof(int));

      if (type == 1)
      {
        read(leftfd, &temp, sizeof(record));

        //Write the results to the pipe the parent opened for us
        write(parentfd, &type, sizeof(int));
        write(parentfd, &temp, sizeof(record));
      }
      else if (type == 0)
      {
        read(leftfd, &sm_2, sizeof(timesSM));
      }
      else if (type == -1)
      {
        //After reading the times we no longer need to read data
        break;
      }
    }

    //Updating the times with the ones we received from the children
    if (sm_1.maxS > sm_2.maxS)
    {
      SM_times.maxS = sm_1.maxS;
    }
    else
    {
      SM_times.maxS = sm_2.maxS;
    }
    if (sm_1.minS < sm_2.minS)
    {
      SM_times.minS = sm_1.minS;
    }
    else
    {
      SM_times.minS = sm_2.minS;
    }
    SM_times.avgS = (sm_1.avgS + sm_2.avgS) / 2;
    if (sm_1.maxSM > sm_2.maxSM)
    {
      SM_times.maxSM = sm_1.maxSM;
    }
    else
    {
      SM_times.maxSM = sm_2.maxSM;
    }
    if (sm_1.minSM < sm_2.minSM)
    {
      SM_times.minSM = sm_1.minSM;
    }
    else
    {
      SM_times.minSM = sm_2.minSM;
    }
    SM_times.avgSM = (sm_1.avgSM + sm_2.avgSM) / 2;

    //We also have to calculate the time of the node we are in
    gettimeofday(&end, NULL);
    total_t = (double) (end.tv_usec - begin.tv_usec) / 1000000;

    if (total_t > SM_times.maxSM)
    {
      SM_times.maxSM = total_t;
    }
    if (total_t < SM_times.minSM)
    {
      SM_times.minSM = total_t;
    }
    SM_times.avgSM = (SM_times.avgSM + total_t) / 2;

    type = 0;
    write(parentfd, &type, sizeof(int));
    write(parentfd, &SM_times, sizeof(timesSM));

    type = -1;
    write(parentfd, &type, sizeof(int));

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
