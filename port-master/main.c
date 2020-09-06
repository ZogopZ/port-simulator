#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>


#include "tools.h"
#include "printer.h"
#include "../myport/checker.h"
#include "../myport/cli_utils.h"
#include "../myport/port.h"

extern sig_atomic_t volatile vesselCreate;
extern int sleepingTime;
extern pid_t pid;

int main(int argc, char** argv)
{
    vesselCreate = true;                            //Terminates the port if false.
    sleepingTime = 0;                               //Sets sleeping values for debugging.
    signal(SIGUSR1, SigHandler);                    //Handle SIGUSR1. Sets vesselCreate to false.
    signal(SIGUSR2, SigHandler);                    //Handle SIGUSR2. Change speed of output.

    int shmid;
    int semD_value = 0, checker = 0, status = 0;
    sharedMemory_t* shMem;

    ParseCliArgs(argc, argv, PORT_MASTER);          //Check in line parameters.
    shmid = atoi(GetStrShmid());
    shMem = (sharedMemory_t*)shmat(shmid, (void*)0, 0);
    check_shmat(shMem, PORT_MASTER);                //Check if shmat() function failed.
    FILE* fpLog = fopen(shMem->logFile, "a");       //Open log file for appending only.
    check_fopen(fpLog, LOG_FILE, PORT_MASTER);      //Cehck if fopen() function failed.
    PrintPortMasterInit(fpLog);                     //__PRINTER.
    checker = sem_init(&shMem->semA, 1, 0);         //Initialize semaphore for sharing between processes (since using execl).
    check_seminit(checker, PORT_MASTER);            //Check if sem_init() function failed.
    checker = sem_init(&shMem->semB, 1, 1);         //Initialize semaphore for sharing between processes (since using execl).
    check_seminit(checker, PORT_MASTER);            //Check if sem_init() function failed.
    checker = sem_init(&shMem->semC, 1, 0);         //Initialize semaphore for sharing between processes (since using execl).
    check_seminit(checker, PORT_MASTER);            //Check if sem_init() function failed.
    checker = sem_init(&shMem->semD, 1, 1);         //Initialize semaphore for sharing between processes (since using execl).
    check_seminit(checker, PORT_MASTER);            //Check if sem_init() function failed.
    checker = sem_init(&shMem->semE, 1, 0);         //Initialize semaphore for sharing between processes (since using execl).
    check_seminit(checker, PORT_MASTER);            //Check if sem_init() function failed.

    pid = fork();                                   //Fork to vessel creator.
    check_fork(pid, PORT_MASTER);                   //Check if fork() function failed.
    if (pid == 0)                   //***Vessel-creator*** child process.
    {
        while (vesselCreate == true)                //Repeat until SIGUSR1 is caught and handled.
        {
            sleep(2);
            CreateRandomVessel();                   //Create a vessel with random,correct arguments.
        }
        PrintPortClosing();                         //__PRINTER.
        checker = wait(&status);                    //Wait for children.
        check_wait(checker, PORT_MASTER);           //Check if wait() function failed.
    }
    else                            //***Port-Master*** process.
    {
        PrintPort(shMem, fpLog);
        while (1)                                   //Repeat until SIGUSR1 is caught and handled and until there are no more vessels in port.
        {
            checker = sem_wait(&shMem->semA);       //Wait for vessel arrival or vessel request to depart.
            check_semwait(checker, PORT_MASTER);    //Check if sem_wait() function failed.
            if (vesselCreate == false)              //SIGUSR1 caught. Port master will not accept any more vessels and will wait for the remaining vessels to leave.
                shMem->function = STOP;
            checker = sem_getvalue(&shMem->semD, &semD_value);
            check_semget(checker, PORT_MASTER);     //Check if sem_getvalue() function failed.
            if (semD_value == 0)                    //Vessel wants to depart from dock.
            {
                PrintUndock(shMem, fpLog);          //__PRINTER.
                UndockVessel(shMem);                //Remove vessel'ss data from it's port spot.
                PrintLeavePortMaster(shMem, fpLog); //__PRINTER.
                checker = sem_post(&shMem->semD);   //Vessel gets approval for departure.
                check_sempost(checker, PORT_MASTER);//Check if sem_post() function failed.
                checker = sem_post(&shMem->semE);   //Use another semaphore so that one vessel departs each time.
                check_sempost(checker, PORT_MASTER);//Check if sem_post() function failed.
                checker = sem_wait(&shMem->semA);   //Wait for vessel's exit maneuver.
                check_semwait(checker, PORT_MASTER);//Check if sem_post() function failed.
            }
            else if (semD_value > 0)                //No vessel want's to depart from port yet.
            {
                PrintRequest(shMem, fpLog);         //__PRINTER.
                SearchDock(shMem, fpLog);           //Search port for empty spots according to vessel's request.
                checker = sem_post(&shMem->semC);   //Inform vessel about DOCK or NO_DOCK.
                check_sempost(checker, PORT_MASTER);//Check if sem_post() function failed.
                checker = sem_wait(&shMem->semA);   //Wait for vessel's action (maneuver or go back to approach lane).
                check_semwait(checker, PORT_MASTER);//Check if sem_wait() function failed.
                if (shMem->vessel.canDock == YES)           //There is an empty spot for the incoming vessel.
                {
                    DockVessel(shMem);                      //Find an empty spot for the incoming vessel.
                    PrintSuccessDock(shMem, fpLog);         //__PRINTER.
                    checker = sem_post(&shMem->semC);       //Vessel is docked and will start operations (parking time).
                    check_sempost(checker, PORT_MASTER);    //Check if sem_post() function failed.
                    checker = sem_wait(&shMem->semA);       //Wait for vessel's parking time start.
                    check_semwait(checker, PORT_MASTER);    //Check if sem_wait() function failed.
                }
                else if (shMem->vessel.canDock == NO)       //Could not find an empty spot for the incoming vessel.
                {
                    PrintVesselWait(shMem, fpLog);          //__PRINTER.
                    shMem->vessel.waitingTime = TIME_OUT;   //Reset waitingTime for the next vessel.
                }
            }
            if (vesselCreate == false && shMem->info.tempFullCap == 0)
                break;                              //Process has caught SIGUSR1 and port is empty. Port-master will exit.
            PrintPort(shMem, fpLog);                //__PRINTER.
            checker = sem_post(&shMem->semB);       //Another vessel can start docking or departing.
            check_sempost(checker, PORT_MASTER);    //Check if sem_post() function failed.
        }
        PrintPort(shMem, fpLog);            //__PRINTER.
        checker = wait(&status);            //Wait for children.
        check_wait(checker, PORT_MASTER);   //Check if wait() function failed.
    }
    checker = shmdt((void*)shMem);          //Detach shared memory from process.
    check_shmdt(checker, PORT_MASTER);      //Check if shmdt() function failed.
    free(GetStrShmid());
    checker = fclose(fpLog);                //Check if fclose() function failed.
    check_fclose(checker, LOG_FILE, PORT_MASTER);
    return 0;
}

