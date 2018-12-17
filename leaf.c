/*
File: leaf.c
Here is my leaf node implementation.
Each leaf node receives a part of the initial binary file, searches through
that part and returns the results. It also returns its rum time.
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

//Correct way to call it: name of file, start, end, pattern, pipe, root pid.
int main(int argc, char **argv){
  record tempRec;
  timesS tempTimes;
  char longBuffer[150], intBuffer[150], floatBuffer[150];
  int i, bytesRead, flag, parentfd, type;
  FILE *ptr;
  struct timeval begin, end;
  double total_t;

  gettimeofday(&begin, NULL);

  ptr = fopen(argv[1], "rb");
  if (ptr == NULL)
  {
    perror("Could not open input binary file");
    exit(5);
  }

  //Starting from the specified start
  if (fseek(ptr, sizeof(record) * atoi(argv[2]), SEEK_SET) != 0)
  {
    perror("Can not go to the specified start of the range");
    exit(4);
  }

  parentfd = open(argv[5], O_WRONLY);

  //Loop until all the records in the given range are processed
  for (i = 0; i < (atoi(argv[3]) - atoi(argv[2])); i++)
  {
    bytesRead = fread(&tempRec, sizeof(record), 1, ptr);
    if (bytesRead > 0)
    {
      //If everything went ok we can start searching for the pattern
      flag = 0;
      sprintf(longBuffer, "%lu", tempRec.custid);
      sprintf(intBuffer, "%d", tempRec.HouseID);
      sprintf(floatBuffer, "%f", tempRec.amount);
      if (strstr(longBuffer, argv[4]) != NULL)
      {
        flag = 1;
      }
      else if (strstr(tempRec.FirstName, argv[4]) != NULL)
      {
        flag = 1;
      }
      else if (strstr(tempRec.LastName, argv[4]) != NULL)
      {
        flag = 1;
      }
      else if (strstr(tempRec.Street, argv[4]) != NULL)
      {
        flag = 1;
      }
      else if (strstr(intBuffer, argv[4]) != NULL)
      {
        flag = 1;
      }
      else if (strstr(tempRec.City, argv[4]) != NULL)
      {
        flag = 1;
      }
      else if (strstr(tempRec.postcode, argv[4]) != NULL)
      {
        flag = 1;
      }
      else if (strstr(floatBuffer, argv[4]) != NULL)
      {
        flag = 1;
      }

      if (flag == 1)
      {
        type = 1;
        write(parentfd, &type, sizeof(int));

        write(parentfd, &tempRec, sizeof(record));
      }
    }
    else
    {
      printf("The given range is out of the file's boundaries\n");
      break;
    }
  }

  gettimeofday(&end, NULL);
  total_t = (double) (end.tv_usec - begin.tv_usec) / 1000000;

  tempTimes.time = total_t;

  type = 0;
  write(parentfd, &type, sizeof(int));
  write(parentfd, &tempTimes, sizeof(timesS));

  //So that the parent can know when to stop
  type = -1;
  write(parentfd, &type, sizeof(int));

  close(parentfd);
  rewind(ptr);
  fclose(ptr);

  kill(atoi(argv[6]), SIGUSR2);

  return 0;
}
