#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../myport/checker.h"
#include "printer.h"

int sleepingTime;
pid_t pid;
char timeBuffer[20];

char* getLogTime(void)  //Returns this type of string : "2018-12-18 01:41:47": to calling function.
{                       //year-month-day hours:minutes:seconds.
    time_t rawtime;
    struct tm* timeinfo;

    memset(timeBuffer, 0, 20);      //Initialize buffer.
    time(&rawtime);
    timeinfo = localtime(&rawtime); //The value of rawtime is broken up into the structure tm and expressed in the local time zone.
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return (timeBuffer);
}

/*************** SIGNAL HANDLER PRINTER *****************/
void PrintSigusr1(void)
{
    printf("\t\t\t\t\t\t\t\t\t\t\t\t|***SIGNAL HANDLER***|: Port Stop\n");
    return;
}

void PrintChangeSpeed(void)
{
    float sleepTime = (float)sleepingTime/1000000;
    printf("\t\t\t\t\t\t\t\t\t\t\t\t|***SIGNAL HANDLER***|: Changing speed to : |%.4f| seconds.\n", sleepTime);
    return;
}

/********************MY-PORT PRINTER ********************/
void PrintMyPortInit(FILE* fpLog)
{
    fprintf(fpLog, "%s: |My-PORT->%ld|: Initialized MY-PORT process.\n",
            getLogTime(), (long int)getpid());
    printf("|MY-PORT|: Hello World!(|%ld|)\n", (long int)getpid());
    return;
}

void PrintPortCreate(FILE *fpLog)
{
    fprintf(fpLog, "%s: |My-PORT->%ld|: Initialized shared memory and port.\n",
            getLogTime(), (long int)getpid());
    printf("|MY-PORT|: Shared memory initialization and port creation.\n");
    return;
}

void PrintPortMasterCreate(FILE* fpLog)
{
    fprintf(fpLog, "%s: |My-PORT->%ld|: Created child process PORT-MASTER.\n",
            getLogTime(), (long int)getpid());
    fflush(fpLog);
    return;
}

void PrintDestroySems(FILE* fpLog)
{
    fprintf(fpLog, "%s: |My-PORT->%ld|: Destroyed semaphores.\n",
            getLogTime(), (long int)getpid());
    printf("|MY-PORT|: Destroying semaphores.\n");
    return;
}

void PrintMarkShmem(FILE* fpLog)
{
    fprintf(fpLog, "%s: |My-PORT->%ld|: Marked the shared memory to be destroyed..\n",
            getLogTime(), (long int)getpid());
    printf("|MY-PORT|: Marking the shared memory to be destroyed, after the last process detaches it.\n");
    return;
}

void PrintErrorMyport(FILE *fpLog)
{
    fprintf(fpLog, "%s: |My-PORT->%ld|: Error. One or more vessel types where not included in configuration file.\n",
            getLogTime(), (long int)getpid());
    printf("|MYPORT|: Error. Please include all vessel types in configuration file.\n");
    return;
}

/***************** PORT-MASTER PRINTER ******************/
void PrintPortMasterInit(FILE* fpLog)
{
    fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Initialized PORT-MASTER process and attached shared memory to it.\n",
            getLogTime(), (long int)getpid());
    printf("|PORT-MASTER|: Hello World!(|%ld|)\n", (long int)getpid());
    return;
}

void PrintPort(sharedMemory_t* shMem, FILE* fpLog)
{
    int i;

    fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Port status   --->\t|", getLogTime(), (long int)getpid());
    printf("\t\t\t\t\t\t\t\t\t\t|PORT-MASTER->%ld|  |VESSEL-CREATOR->%ld|  |shmid->%d|\n\t\t\t\t\t\t\t\t\t\t|",
           (long int)getpid(), (long int)pid, shMem->shmid);
    for (i = 0; i < shMem->info.fullCap; i++)
    {
        if (i != shMem->info.fullCap - 1)
        {
            fprintf(fpLog, "******");
            printf("******");
        }
        else if (i == shMem->info.fullCap - 1)
        {
            fprintf(fpLog, "*****");
            printf("*****");
        }
    }
    fprintf(fpLog, "|\n");
    fprintf(fpLog, "\t\t\t\t\t\t\t\t|");
    printf("|\n\t\t\t\t\t\t\t\t\t\t|");
    for (i = 0; i < shMem->info.smallCap; i++)
    {
        fprintf(fpLog, "  S  |");
        printf("  S  |");
    }
    for (i = shMem->info.smallCap; i < shMem->info.maxMedium; i++)
    {
        fprintf(fpLog, "  M  |");
        printf("  M  |");
    }
    for (i = shMem->info.maxMedium; i < shMem->info.maxLarge; i++)
    {
        fprintf(fpLog, "  L  |");
        printf("  L  |");
    }
    fprintf(fpLog, "\n\t\t\t\t\t\t\t\t|");
    printf("\n\t\t\t\t\t\t\t\t\t\t|");
    for (i = 0; i < shMem->info.fullCap; i++)
    {
        if (shMem->port[i].pid == NOPID)
        {
            fprintf(fpLog, "EMPTY|");
            printf("EMPTY|");
        }
        else
        {
            fprintf(fpLog, "%5d|", shMem->port[i].pid);
            printf("%5d|", shMem->port[i].pid);
        }
    }
    fprintf(fpLog, "\n\t\t\t\t\t\t\t\t|");
    printf("\n\t\t\t\t\t\t\t\t\t\t|");
    for (i = 0; i < shMem->info.fullCap; i++)
    {
        if (i != shMem->info.fullCap - 1)
        {
            fprintf(fpLog, "******");
            printf("******");
        }
        else if (i == shMem->info.fullCap - 1)
        {
            fprintf(fpLog, "*****");
            printf("*****");
        }
    }
    fprintf(fpLog, "|\n\n");
    printf("|\n");
    usleep(sleepingTime);
    return;
}

void PrintVesselCreator(void)
{
    printf("|VESSEL-CREATOR|: Hello World!(|%ld|)\n", (long int)getpid());
    return;
}

void PrintPortClosing(void)
{
    printf("|PORT-MASTER|: Port is closing. Remaining vessels will depart.\n");
    usleep(sleepingTime);
    return;
}

void PrintRequest(sharedMemory_t* shMem, FILE* fpLog)
{
    fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld asked permission to dock.\n",
            getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
    printf("|PORT-MASTER|: Vessel %ld wants to dock.\n", (long int)shMem->vessel.pid);
    usleep(sleepingTime);
    return;
}

void PrintFullPort(sharedMemory_t* shMem, FILE* fpLog)
{
    fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld could not dock because the port was full. Port-master sent it back to approach lane.\n",
            getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
    printf("|PORT-MASTER|: Vessel %ld please go back to approach lane. Port is FULL!\n", (long int)shMem->vessel.pid);
    return;
}

void PrintFullType(int type, sharedMemory_t* shMem, FILE* fpLog)    //Spots specified by type are full.
{
    if (shMem->vessel.type == SMALL && type == SMALL)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Could not find a small type parking spot for vessel %ld. Port-master sent it back to approach lane\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
        printf("|PORT-MASTER|: Vessel %ld Small type parking spots are full! Please go back to approach lane.\n",
               (long int)shMem->vessel.pid);
    }
    else if (shMem->vessel.type == SMALL && type == MEDIUM)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Could not find a medium type parking spot for vessel %ld. Port-master checked for small type parking spots.\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
        printf("|PORT-MASTER|: Vessel %ld Medium type parking spots are full! Checking for small type parking spots...\n",
               (long int)shMem->vessel.pid);
    }
    else if (shMem->vessel.type == SMALL && type == LARGE)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Could not find a large type parking spot for vessel %ld. Port-master checked for medium type parking spots.\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
        printf("|PORT-MASTER|: Vessel %ld Large type parking spots are full! Checking for medium type parking spots...\n",
               (long int)shMem->vessel.pid);
    }
    else if (shMem->vessel.type == MEDIUM && type == MEDIUM)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Could not find a medium type parking spot for vessel %ld. Port-master sent it back to approach lane.\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
        printf("|PORT-MASTER|: Vessel %ld Medium type parking spots are full! Please go back to approach lane.\n",
               (long int)shMem->vessel.pid);
    }
    else if (shMem->vessel.type == MEDIUM && type == LARGE)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Could not find a large type parking spot for vessel %ld. Port-master checked for medium type parking spots.\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
        printf("|PORT-MASTER|: Vessel %ld Large type parking spots are full! Checking for medium type parking spots...\n",
               (long int)shMem->vessel.pid);
    }
    else if (shMem->vessel.type == LARGE && type == LARGE)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Could not find a large type parking spot for vessel %ld. Port-master sent it back to approach lane.\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
        printf("|PORT-MASTER|: Vessel %ld Large type parking spots are full! Please go back to approach lane.\n", (long int)shMem->vessel.pid);
    }
    usleep(sleepingTime);
    return;
}

void PrintVesselWait(sharedMemory_t* shMem, FILE* fpLog)
{
    fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld went back to approach lane and waited for %d seconds.\n",
            getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid, shMem->vessel.waitingTime);
    printf("|PORT-MASTER|: Vessel %ld please wait %d seconds.\n",
           (long int)shMem->vessel.pid, shMem->vessel.waitingTime);
    usleep(sleepingTime);
    return;
}

void PrintPortType(sharedMemory_t* shMem, FILE* fpLog)
{
    if (shMem->vessel.dockSpot == SMALL)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld was granted permission to maneuver to small type parking spot (maneuver time: %d seconds).\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid, shMem->vessel.manTime);
        printf("|PORT-MASTER|: Vessel %ld you are free to maunever to SMALL spot.\n", (long int)shMem->vessel.pid);
    }
    else if (shMem->vessel.dockSpot == MEDIUM)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld was granted permission to maneuver to medium type parking spot (maneuver time: %d seconds).\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid, shMem->vessel.manTime);
        printf("|PORT-MASTER|: Vessel %ld you are free to maunever to MEDIUM spot.\n", (long int)shMem->vessel.pid);
    }
    else if (shMem->vessel.dockSpot == LARGE)
    {
        fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld was granted permission to maneuver to large type parking spot (maneuver time: %d seconds).\n",
                getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid, shMem->vessel.manTime);
        printf("|PORT-MASTER|: Vessel %ld you are free to maunever to LARGE spot.\n", (long int)shMem->vessel.pid);
    }
    usleep(sleepingTime);
    return;
}

void PrintSuccessDock(sharedMemory_t* shMem, FILE* fpLog)
{
    fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld was successfully docked (parking time: %d seconds).\n",
            getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid, shMem->vessel.parkPeriod);
    printf("|PORT-MASTER|: Vessel %ld successfully docked.(parking time: %d seconds)\n",
           (long int)shMem->vessel.pid, shMem->vessel.parkPeriod);
    usleep(sleepingTime);
    return;
}

void PrintUndock(sharedMemory_t* shMem, FILE* fpLog)
{
    fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld asked permission to depart.\n",
            getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid);
    return;
}

void PrintLeavePortMaster(sharedMemory_t* shMem, FILE* fpLog)
{
    fprintf(fpLog, "%s: |PORT-MASTER->%ld|: Vessel %ld started exiting maneuver (%d seconds).\n",
            getLogTime(), (long int)getpid(), (long int)shMem->vessel.pid, shMem->vessel.manTime);
    printf("|PORT-MASTER|: Vessel %ld please start exiting maneuver.\n", (long int)shMem->vessel.pid);
    usleep(sleepingTime);
    return;
}


/******************** VESSEL PRINTER ********************/
void PrintGoBack(sharedMemory_t* shMem)
{
    printf("\t     -|VESSEL-%ld|: Going back to approach lane.\n", (long int)shMem->vessel.pid);
    usleep(sleepingTime);
    return;
}

void PrintWillToDock(sharedMemory_t* shMem)
{
    if (shMem->vessel.type == SMALL && shMem->vessel.upgradeTo == NO_UPGRADE)
        printf("\n\t     -|VESSEL-%ld|: I want to dock (type SMALL).\n", (long int)shMem->vessel.pid);
    else if (shMem->vessel.type == SMALL && shMem->vessel.upgradeTo == MEDIUM)
        printf("\n\t     -|VESSEL-%ld|: I want to dock (type SMALL willing to pay for MEDIUM).\n", (long int)shMem->vessel.pid);
    else if (shMem->vessel.type == SMALL && shMem->vessel.upgradeTo == LARGE)
        printf("\n\t     -|VESSEL-%ld|: I want to dock (type SMALL willing to pay for LARGE).\n", (long int)shMem->vessel.pid);
    else if (shMem->vessel.type == MEDIUM && shMem->vessel.upgradeTo == NO_UPGRADE)
        printf("\n\t     -|VESSEL-%ld|: I want to dock (type MEDIUM).\n", (long int)shMem->vessel.pid);
    else if (shMem->vessel.type == MEDIUM && shMem->vessel.upgradeTo == LARGE)
        printf("\n\t     -|VESSEL-%ld|: I want to dock (type MEDIUM willing to pay for LARGE).\n", (long int)shMem->vessel.pid);
    else if (shMem->vessel.type == LARGE && shMem->vessel.upgradeTo == NO_UPGRADE)
        printf("\n\t     -|VESSEL-%ld|: I want to dock (type LARGE).\n", (long int)shMem->vessel.pid);
    usleep(sleepingTime);
    return;
}

void PrintManeuver(sharedMemory_t* shMem)
{
    printf("\t     -|VESSEL-%ld|: Maneuvering to port.(maneuvering time: %d seconds)\n",
           (long int)shMem->vessel.pid, shMem->vessel.manTime);
    return;
}

void PrintLeaveVessel(sharedMemory_t* shMem)
{
    printf("\n\t     -|VESSEL-%ld|: Job's done. Asking Permission to depart.\n", (long int)shMem->vessel.pid);
    usleep(sleepingTime);
    return;
}

void PrintBye(sharedMemory_t* shMem)
{
    printf("\t     -|VESSEL-%ld|: BYE BYE!\n", (long int)shMem->vessel.pid);
    usleep(sleepingTime);
    return;
}

