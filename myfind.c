/*
File: myfind.c
Here is the main of my app.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "tree_interface.h"

int main(int argc, char **argv){
  int flag_sum = 0, skew_flag = 0, height, i;
  char *input, *pattern;
  int arr[12] = {1, 3, 4, 654, 234, 543, 34, 54, 34, 45, 34, 34};
  int range[2];
  range[0] = 0;
  range[1] = 11;

  //checking the input from the command line
  if (argc > 8)
  {
    printf("Wrong arguments\n");
    exit(2);
  }
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-s") == 0)
    {
      skew_flag = 1;
    }
    else if (strcmp(argv[i], "-h") == 0)
    {
      height = atoi(argv[i + 1]);
      if (height < 1)
      {
        printf("Height should be at least 1\n");
        exit(2);
      }
      flag_sum++;
      i++;
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      // input = malloc(strlen(argv[i + 1]) + 1);
      // if (input == NULL)
      // {
      //   perror("Unable to allocate space");
      //   exit(-1);
      // }
      //
      // strcpy(input, argv[i + 1]);

      flag_sum++;
      i++;
    }
    else if (strcmp(argv[i], "-p") == 0)
    {
      pattern = malloc(strlen(argv[i + 1]) + 1);
      if (pattern == NULL)
      {
        perror("Unable to allocate space");
        exit(-1);
      }

      strcpy(pattern, argv[i + 1]);

      flag_sum++;
      i++;
    }
    else
    {
      printf("Only use the following flags: -h, -d, -p or -s\n");
      exit(2);
    }
  }
  if (flag_sum != 3)
  {
    printf("Flags: -h, -d and -p are mandatory\n");
    exit(2);
  }

  splitterMerger(arr, range, height);

  return 0;
}
