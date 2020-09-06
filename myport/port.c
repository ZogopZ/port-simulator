#include <string.h>

#include "port.h"
#include "tools.h"
#include "checker.h"


void PortCreation(fileData_t data, sharedMemory_t* initMem)
{
    int i;

    /***Populate struct with data, according to configuration file.***/
    initMem->function = GO_ON;
    initMem->info.fullCap = data.portCapacity;
    initMem->info.smallCap = data.smallCap;
    initMem->info.mediumCap = data.mediumCap;
    initMem->info.largeCap = data.largeCap;
    initMem->info.maxMedium = data.smallCap + data.mediumCap;
    initMem->info.maxLarge = data.smallCap + data.mediumCap + data.largeCap;
    for (i = 0; i < data.smallCap; i++)
    {//Initialize small type port spots.
        initMem->port[i].pid = NOPID;
        initMem->port[i].spotType = SMALL;
        initMem->port[i].vesselType = NO_TYPE;
        initMem->port[i].cost = data.smallCost;
        initMem->port[i].parkPeriod = TIME_OUT;
        initMem->port[i].manTime = TIME_OUT;
    }
    for (i = data.smallCap; i < data.smallCap + data.mediumCap; i++)
    {//Initialize medium type port spots.
        initMem->port[i].pid = NOPID;
        initMem->port[i].spotType = MEDIUM;
        initMem->port[i].vesselType = NO_TYPE;
        initMem->port[i].cost = data.mediumCost;
        initMem->port[i].parkPeriod = TIME_OUT;
        initMem->port[i].manTime = TIME_OUT;
    }
    for (i = data.smallCap + data.mediumCap;
         i < data.smallCap + data.mediumCap + data.largeCap; i++)
    {//Initialize large type port spots.
        initMem->port[i].pid = NOPID;
        initMem->port[i].spotType = LARGE;
        initMem->port[i].vesselType = NO_TYPE;
        initMem->port[i].cost = data.largeCost;
        initMem->port[i].parkPeriod = TIME_OUT;
        initMem->port[i].manTime = TIME_OUT;
    }
    initMem->timeType = data.timeType;
    initMem->timePeriod = data.timePeriod;
    memcpy(initMem->logFile, "logFile", 8);
    return;
}

void DestroySemaphores(sharedMemory_t* shMem)
{
    int checker = 0;
    checker = sem_destroy(&shMem->semA);    //Destroy the semaphore.
    check_semdestroy(checker, MYPORT);      //Check if sem_destroy() function failed.
    checker = sem_destroy(&shMem->semB);
    check_semdestroy(checker, MYPORT);
    checker = sem_destroy(&shMem->semC);
    check_semdestroy(checker, MYPORT);
    checker = sem_destroy(&shMem->semD);
    check_semdestroy(checker, MYPORT);
    checker = sem_destroy(&shMem->semE);
    check_semdestroy(checker, MYPORT);
    return;
}

int CountDigits(int number)
{
    int count = 0;

    while(number != 0)
    {
        number = number/10;
        count++;
    }
    return(count);
}
