//File: mytypes.h
//I'm defining here all the types I need

#ifndef MYTYPES_H
#define MYTYPES_H

//The struct that holds the records as specified
typedef struct record{
    long custid;
    char FirstName[20];
    char LastName[20];
    char Street[20];
    int HouseID;
    char City[20];
    char postcode[6];
    float amount;
} record;

//Each searcher will send one of these structs to their parent
typedef struct timesS{
  double time;
} timesS;

//Each splitterMerger node will send one of these structs
//and every node will update it appropriately
typedef struct timesSM{
  double maxS;
  double minS;
  double avgS;
  double maxSM;
  double minSM;
  double avgSM;
} timesSM;

#endif
