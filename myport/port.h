#ifndef _PORT_H_
#define _PORT_H_

#include <stdio.h>
#include <semaphore.h>

#define NOT_DOCKED  -100
#define DOCKED      -99
#define NO_TYPE     -98
#define NO_UPGRADE  -97
#define SMALL       -96
#define MEDIUM      -95
#define LARGE       -94
#define ISEMPTY     -93
#define ISFULL      -92
#define NOPID       -91
#define TIME_OUT    -90
#define MINUTES     -89
#define HOURS       -88
#define GO_ON       -87
#define STOP        -86
#define YES         -85
#define NO          -84

typedef struct
{
    int typeSmall;
    int typeMedium;
    int typeLarge;
    int smallCap;
    int mediumCap;
    int largeCap;
    int smallCost;
    int mediumCost;
    int largeCost;
    int timePeriod;
    int timeType;
    int portCapacity;

} fileData_t;

typedef struct
{
    int canDock;
    int dockSpot;
    pid_t pid;
    int type;
    int upgradeTo;
    int parkPeriod;
    int manTime;
    int waitingTime;
} vessel_t;

typedef struct
{
    int fullCap;
    int tempFullCap;
    int smallCap;
    int tempSmallCap;
    int mediumCap;
    int tempMediumCap;
    int largeCap;
    int tempLargeCap;
    int maxMedium;
    int maxLarge;
} info_t;

typedef struct
{
    pid_t pid;
    int spotType;
    int vesselType;
    int cost;
    int parkPeriod;
    int manTime;
} portSpot_t;

typedef struct
{
    int function;
    int shmid;
    int timeType;
    int timePeriod;
    char logFile[8];
    sem_t semA;
    sem_t semB;
    sem_t semC;
    sem_t semD;
    sem_t semE;
    info_t info;
    vessel_t vessel;
    portSpot_t port[0];
} sharedMemory_t;

void PortCreation(fileData_t configData, sharedMemory_t* initMemory);
void DestroySemaphores(sharedMemory_t* shMem);
int CountDigits(int number);

#endif /* _PORT_H_ */
