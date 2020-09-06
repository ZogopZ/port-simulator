#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#include "tools.h"
#include "../myport/checker.h"
#include "../myport/cli_utils.h"
#include "printer.h"

sig_atomic_t volatile vesselCreate;
int sleepingTime;

void SearchDock(sharedMemory_t* shMem, FILE* fpLog)
{
    int type = shMem->vessel.type;
    int upgradeType = shMem->vessel.upgradeTo;

    if (shMem->info.tempFullCap == shMem->info.fullCap)     //In case port is full, do not permit vessel to dock.
    {
        PrintFullPort(shMem, fpLog);                        //__PRINTER.
        shMem->vessel.canDock = NO;
    }
    else if (shMem->info.tempFullCap < shMem->info.fullCap) //There are empty spots. Check what types of port spots are available.
    {
        if (type == SMALL && upgradeType == NO_UPGRADE)
            SearchSmallType(shMem, fpLog);
        else if (type == SMALL && upgradeType == MEDIUM)
            SearchMediumType(shMem, fpLog);
        else if (type == SMALL && upgradeType == LARGE)
            SearchLargeType(shMem, fpLog);
        else if (type == MEDIUM && upgradeType == NO_UPGRADE)
            SearchMediumType(shMem, fpLog);
        else if (type == MEDIUM && upgradeType == LARGE)
            SearchLargeType(shMem, fpLog);
        else if (type == LARGE)
            SearchLargeType(shMem, fpLog);
    }
    if (shMem->vessel.canDock == NO)
        GetWaitingTime(shMem);
    return;
}

void SearchSmallType(sharedMemory_t* shMem, FILE* fpLog)
{
    if (shMem->info.tempSmallCap < shMem->info.smallCap)            //There are empty small type port spots.
    {
        shMem->vessel.canDock = YES;
        shMem->vessel.dockSpot = SMALL;
        PrintPortType(shMem, fpLog);                                //__PRINTER.
    }
    else if (shMem->info.tempSmallCap == shMem->info.smallCap)      //There aren't any empty small type port spots.
    {
        PrintFullType(SMALL, shMem, fpLog);
        shMem->vessel.canDock = NO;
    }
    return;
}

void SearchMediumType(sharedMemory_t* shMem, FILE* fpLog)
{
    if (shMem->info.tempMediumCap < shMem->info.mediumCap)          //There are empty medium type port spots.
    {
        shMem->vessel.canDock = YES;
        shMem->vessel.dockSpot = MEDIUM;
        PrintPortType(shMem, fpLog);                                //__PRINTER.
    }
    else if (shMem->info.tempMediumCap == shMem->info.mediumCap)    //There aren't any empty medium type port spots.
    {
        PrintFullType(MEDIUM, shMem, fpLog);
        if (shMem->vessel.type == MEDIUM)
            shMem->vessel.canDock = NO;
        else if (shMem->vessel.type == SMALL)
            SearchSmallType(shMem, fpLog);                          //Check for small types as well, according to vessel's request.
    }
    return;
}

void SearchLargeType(sharedMemory_t* shMem, FILE* fpLog)
{
    if (shMem->info.tempLargeCap < shMem->info.largeCap)            //There are empty large type port spots.
    {
        shMem->vessel.canDock = YES;
        shMem->vessel.dockSpot = LARGE;
        PrintPortType(shMem, fpLog);                                //__PRINTER.
    }
    else if (shMem->info.tempLargeCap == shMem->info.largeCap)      //There aren't any empty large type port spots.
    {
        PrintFullType(LARGE, shMem, fpLog);
        if (shMem->vessel.type == LARGE)
            shMem->vessel.canDock = NO;
        else if (shMem->vessel.type == MEDIUM || shMem->vessel.type == SMALL)
            SearchMediumType(shMem, fpLog);                         //Check for medium types as well, according to vessel's request.
    }
    return;
}

void DockVessel(sharedMemory_t* shMem)          //Empty spots found. Docking is possible.
{
    int emptySpot;
    if (shMem->vessel.dockSpot == SMALL)        //There is certainly one or more free small type spots (because of previous checks).
    {
        for (int i = 0; i < shMem->info.smallCap; i++)
        {                                       //Find an empty small type port spot.
            if (shMem->port[i].vesselType == NO_TYPE)
            {
                emptySpot = i;
                break;
            }
        }                                       //Fill port spot with vessel's data.
        shMem->port[emptySpot].pid = shMem->vessel.pid;
        shMem->port[emptySpot].vesselType = shMem->vessel.type;
        shMem->port[emptySpot].cost = shMem->vessel.parkPeriod;
        shMem->port[emptySpot].parkPeriod = shMem->vessel.parkPeriod;
        shMem->port[emptySpot].manTime = shMem->vessel.manTime;
        shMem->vessel.canDock = YES;
        shMem->info.tempSmallCap++;
        shMem->info.tempFullCap++;
    }
    else if (shMem->vessel.dockSpot == MEDIUM)  //There is certainly one or more free medium type spots (because of previous checks).
    {
        for (int i = shMem->info.smallCap; i < shMem->info.maxMedium; i++)
        {                                       //Find an empty medium type port spot.
            if (shMem->port[i].vesselType == NO_TYPE)
            {
                emptySpot = i;
                break;
            }
        }                                       //Fill port spot with vessel's data.
        shMem->port[emptySpot].pid = shMem->vessel.pid;
        shMem->port[emptySpot].vesselType = shMem->vessel.type;
        shMem->port[emptySpot].cost = shMem->vessel.parkPeriod;      //TODO parkPeriod*timi(ana lepto?).
        shMem->port[emptySpot].parkPeriod = shMem->vessel.parkPeriod;
        shMem->port[emptySpot].manTime = shMem->vessel.manTime;
        shMem->vessel.canDock = YES;
        shMem->info.tempMediumCap++;
        shMem->info.tempFullCap++;
    }
    else if (shMem->vessel.dockSpot == LARGE)   //There is certainly one or more free large type spots (because of previous checks).
    {
        for (int i = shMem->info.maxMedium; i < shMem->info.maxLarge; i++)
        {                                       //Find an empty large type port spot.
            if (shMem->port[i].vesselType == NO_TYPE)
            {
                emptySpot = i;
                break;
            }
        }                                       //Fill port spot with vessel's data.
        shMem->port[emptySpot].pid = shMem->vessel.pid;
        shMem->port[emptySpot].vesselType = shMem->vessel.type;
        shMem->port[emptySpot].cost = shMem->vessel.parkPeriod;      //TODO parkPeriod*timi(ana lepto?).
        shMem->port[emptySpot].parkPeriod = shMem->vessel.parkPeriod;
        shMem->port[emptySpot].manTime = shMem->vessel.manTime;
        shMem->vessel.canDock = YES;
        shMem->info.tempLargeCap++;
        shMem->info.tempFullCap++;
    }
    return;
}

void UndockVessel(sharedMemory_t* shMem)
{
    for (int i = 0; i < shMem->info.fullCap; i++)       //Search vessel's pid inside port.
    {
        if (shMem->port[i].pid == shMem->vessel.pid)
        {
            shMem->port[i].pid = NOPID;                 //Reinitialize port spot's data.
            shMem->port[i].vesselType = NO_TYPE;
            shMem->port[i].parkPeriod = TIME_OUT;
            shMem->port[i].manTime = TIME_OUT;
            if (shMem->port[i].spotType == SMALL)       //Each time a vessel is leaving decrement specific counters.
                shMem->info.tempSmallCap--;             //This way we can access data quicker.
            else if (shMem->port[i].spotType == MEDIUM)
                shMem->info.tempMediumCap--;
            else if (shMem->port[i].spotType == LARGE)
                shMem->info.tempLargeCap--;
            shMem->info.tempFullCap--;
            break;
        }
    }
    return;
}

void GetWaitingTime(sharedMemory_t* shMem)          //Port master couldn't find an empty spot. Calculate waiting time for it.
{
    int i = 0, tempTime = 0;
    int type = shMem->vessel.type;
    int upgradeType = shMem->vessel.upgradeTo;

    //Get the minimum parking period time of vessels already in port.
    if (type == SMALL && upgradeType == NO_UPGRADE)         //Check parking times for small type port spots.
    {
        for (i = 0; i < shMem->info.smallCap; i++)
        {
            if (shMem->port[i].parkPeriod != TIME_OUT && tempTime == 0)
                tempTime = shMem->port[i].parkPeriod;
            else if (shMem->port[i].parkPeriod < tempTime)
                tempTime = shMem->port[i].parkPeriod;
        }
    }
    else if (type == SMALL && upgradeType == MEDIUM)        //Check parking times for small and medium type port spots.
    {
        for (i = 0; i < shMem->info.maxMedium; i++)
        {
            if (shMem->port[i].parkPeriod != TIME_OUT && tempTime == 0)
                tempTime = shMem->port[i].parkPeriod;
            else if (shMem->port[i].parkPeriod < tempTime)
                tempTime = shMem->port[i].parkPeriod;
        }
    }
    else if (type == SMALL && upgradeType == LARGE)         //Check parking times for small, medium and large type port spots.
    {
        for (i = 0; i < shMem->info.maxLarge; i++)
        {
            if (shMem->port[i].parkPeriod != TIME_OUT && tempTime == 0)
                tempTime = shMem->port[i].parkPeriod;
            else if (shMem->port[i].parkPeriod < tempTime)
                tempTime = shMem->port[i].parkPeriod;
        }
    }
    else if (type == MEDIUM && upgradeType == NO_UPGRADE)   //Check parking times for medium type port spots.
    {
        for (i = shMem->info.smallCap; i < shMem->info.maxMedium; i++)
        {
            if (shMem->port[i].parkPeriod != TIME_OUT && tempTime == 0)
                tempTime = shMem->port[i].parkPeriod;
            else if (shMem->port[i].parkPeriod < tempTime)
                tempTime = shMem->port[i].parkPeriod;
        }
    }
    else if (type == MEDIUM && upgradeType == LARGE)        //Check parking times for medium and large type port spots.
    {
        for (i = shMem->info.smallCap; i < shMem->info.maxLarge; i++)
        {
            if (shMem->port[i].parkPeriod != TIME_OUT && tempTime == 0)
                tempTime = shMem->port[i].parkPeriod;
            else if (shMem->port[i].parkPeriod < tempTime)
                tempTime = shMem->port[i].parkPeriod;
        }
    }
    else if (type == LARGE)                                 //Check parking times for large type port spots.
    {
        for (i = shMem->info.maxMedium; i < shMem->info.maxLarge; i++)
        {
            if (shMem->port[i].parkPeriod != TIME_OUT && tempTime == 0)
                tempTime = shMem->port[i].parkPeriod;
            else if (shMem->port[i].parkPeriod < tempTime)
                tempTime = shMem->port[i].parkPeriod;
        }
    }
    shMem->vessel.waitingTime = tempTime;
    return;
}

void CreateRandomVessel(void)
{
    pid_t pid;
    int type, posType, parkPeriod, manTime;
    char* strType, * strPosType, * strParkPeriod, * strManTime;

    srand(time(NULL));          //Initialize random number generator.
    type = rand()%3;            //Get a random vessel type.
    posType = rand()%2;         //Get a random 0 or 1.
    if (type == 2)              //Largest type availabe cannot upgrade.
        posType = -1;
    else if (posType == 0)      //Random case 0. Vessel cannot upgrade.
        posType = -1;
    else if (posType == 1)      //Random case 1. Vessel can upgrade.
    {
        while ((posType = (rand()%2 + 1)))
        {
            if (posType > type) //Position type must be greater that vessel type.
                break;
            else
                continue;
        }
    }
    parkPeriod = rand()%20 + 5; //Get a random parking period, 5-24 seconds.
    manTime = rand()%4 + 1;     //Get a random maneuver time. 1-4 seconds.

    if (type == 0)              //Assign type according to random number generated.
        type = SMALL;
    else if (type == 1)
        type = MEDIUM;
    else if (type == 2)
        type = LARGE;

    if (posType == 1)           //Assign upgrade type according to random number generated.
        posType = MEDIUM;
    else if (posType == 2)
        posType = LARGE;
    else if (posType == -1)
        posType = NO_UPGRADE;

    strType = mallocNcheck(3, PORT_MASTER); //Allocate memory for strings.
    strPosType = mallocNcheck(3, PORT_MASTER);
    strParkPeriod = mallocNcheck(CountDigits(parkPeriod), PORT_MASTER);
    strManTime = mallocNcheck(CountDigits(manTime), PORT_MASTER);

    sprintf(strType, "%d", type);           //Strings to be used with execl() function.
    sprintf(strPosType, "%d", posType);
    sprintf(strParkPeriod, "%d", parkPeriod);
    sprintf(strManTime, "%d", manTime);

    pid = fork();                           //Fork to random vessel.
    check_fork(pid, PORT_MASTER);           //Check if execl() function succeeded.
    if (pid == 0)                           //***Random Vessel*** child process.
    {
        execl("./vessel/vessel", "",
              "-t", strType,
              "-u", strPosType,
              "-p", strParkPeriod,
              "-m", strManTime,
              "-s", GetStrShmid(), (char*)NULL);
        check_exec(PORT_MASTER);            //Check if execl() function succeeded.
    }
    free(strType);
    free(strPosType);
    free(strParkPeriod);
    free(strManTime);
    return;
}

void SigHandler(int signo)
{
    if (signo == SIGUSR1)
    {
        PrintSigusr1();                     //__PRINTER.
        vesselCreate = false;               //Vessel creator will shut down, port master will wait for vessels to leave and will exit.
    }
    else if (signo == SIGUSR2)
    {
        if (sleepingTime == 0)
        {
            PrintChangeSpeed();             //__PRINTER.
            sleepingTime = 500000;
        }
        else if (sleepingTime == 500000)
        {
            PrintChangeSpeed();             //__PRINTER.
            sleepingTime = 1000000;
        }
        else if (sleepingTime == 1000000)
        {
            PrintChangeSpeed();             //__PRINTER.
            sleepingTime = 2000000;
        }
        else if (sleepingTime == 2000000)
        {
            PrintChangeSpeed();             //__PRINTER.
            sleepingTime = 3000000;
        }
        else if (sleepingTime == 3000000)
        {
            PrintChangeSpeed();             //__PRINTER.
            sleepingTime = 0;
        }
    }
}
