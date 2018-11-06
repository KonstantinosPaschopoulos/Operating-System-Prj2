//File: mytypes.h
//I'm defining here all the types I need

#ifndef MYTYPES_H
#define MYTYPES_H

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

#endif
