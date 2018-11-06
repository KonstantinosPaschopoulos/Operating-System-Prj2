/*
File: leaf.c
Here is my leaf node implementation.
Each leaf node receives a part of the initial binary file, searches through
that part and returns the results. It also returns its rum time.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "mytypes.h"

//Correct way to call it: name of file, start, end, pattern
int main(int argc, char **argv){
  record tempRec;
  char longBuffer[150];
  char intBuffer[150];
  char floatBuffer[150];
  int i, bytesRead, flag;
  FILE *ptr;

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


  //Loop until all the records the range specifies are processed
  for (i = 0; i < (atoi(argv[3]) - atoi(argv[2])); i++)
  {
    bytesRead = fread(&tempRec, sizeof(record), 1, ptr);
    if (bytesRead > 0)
    {
      //If everything went ok we can start searching for the pattern
      flag = 0;
      snprintf(longBuffer, sizeof(long), "%lu", tempRec.custid);
      snprintf(intBuffer, sizeof(int), "%d", tempRec.HouseID);
      snprintf(floatBuffer, sizeof(float), "%f", tempRec.amount);
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
        printf("%lu %s %s ", tempRec.custid, tempRec.FirstName, tempRec.LastName);
        printf("%s %d %s ", tempRec.Street, tempRec.HouseID, tempRec.City);
        printf("%s %f\n", tempRec.postcode, tempRec.amount);
      }
    }
    else
    {
      printf("The given range is out of the file's boundaries\n");
      break;
    }
  }

  fclose(ptr);

  return 0;
}
