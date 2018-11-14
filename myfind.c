/*
File: myfind.c
Here is the main function of my app.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv){
  pid_t root;
  int flag_sum = 0, height, i, status, skew_flag;
  char input[200], pattern[200], heightStr[150], skew[150];
  struct timeval begin, end;
  double total_t;

  gettimeofday(&begin, NULL);

  skew_flag = 0;

  //Checking the input from the command line
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
      if (height > 5)
      {
        printf("Height can not be more than 5\n");
        exit(2);
      }

      //Turning the height into a string in order to give it
      //as an argument to the root process
      snprintf(heightStr, sizeof(int), "%d", height);

      flag_sum++;
      i++;
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      strcpy(input, argv[i + 1]);

      flag_sum++;
      i++;
    }
    else if (strcmp(argv[i], "-p") == 0)
    {
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

  root = fork();
  if (root < 0)
  {
    perror("Root Fork Failed");
    exit(1);
  }

  if (root == 0)
  {
    snprintf(skew, sizeof(int), "%d", skew_flag);
    
    //Calling the root process to start the whole thing
    execl("root", "root", input, pattern, heightStr, skew, NULL);

    perror("Root failed to exec");
    exit(1);
  }

  //After the root is finished, everything should be done
  wait(&status);

  gettimeofday(&end, NULL);
  total_t = (double) (end.tv_usec - begin.tv_usec) / 1000000;

  printf("Turnaround Time %f\n", total_t);

  return 0;
}
