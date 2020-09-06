#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>

#include "../myport/checker.h"
#include "../myport/cli_utils.h"
#include "../port-master/printer.h"
#include "tools.h"


int main(int argc, char** argv)
{
    int shmid;
    int checker;
    int waitingTime = 0;
    int parkingTime = 0;
    sharedMemory_t* shMem;

    ParseCliArgs(argc, argv, VESSEL);           //Check in line parameters.
    shmid = atoi(GetStrShmid());
    shMem = (sharedMemory_t*)shmat(shmid, (void*)0, 0);  //Attach shared memory.
    check_shmat(shMem, VESSEL);                          //Check if shmat() function failed.
    int vessel = NOT_DOCKED;
    while (vessel == NOT_DOCKED)                //Repeat while vessel is not yet docked.
    {
        /****Initial semB value is 1.***/
        checker = sem_wait(&shMem->semB);       //Vessels wait for entering or departing from port.
        check_semwait(checker, VESSEL);         //Check if sem_wait() function failed.
        if (shMem->function == STOP)            //Port received SIGUSR1 and must close.
        {
            checker = sem_post(&shMem->semB);   //All vessels that just arrived or were waiting for entering approval, must leave.
            check_sempost(checker, VESSEL);     //Check if sem_post() function failed.
            break;
        }
        DataForPortMaster(shMem);               //Write vessel's info to shared memory. Port master will then evaluate it.
        PrintWillToDock(shMem);                 //__PRINTER.
        checker = sem_post(&shMem->semA);       //Port master will evaluate data given data from vessel.
        check_sempost(checker, VESSEL);         //Check if sem_post() function failed.
        checker = sem_wait(&shMem->semC);       //Wait for port-master's approval or disapproval for entering port.
        check_semwait(checker, VESSEL);         //Check if sem_wait() function failed.
        if (shMem->vessel.canDock == YES)       //Vessel can dock.
        {
            vessel = DOCKED;
            PrintManeuver(shMem);               //__PRINTER.
            sleep(shMem->vessel.manTime);       //Vessel is maneuvering (entering port).
            checker = sem_post(&shMem->semA);   //Vessel arrived at the empty spot.
            check_sempost(checker, VESSEL);     //Check if sem_post() function failed.
            checker = sem_wait(&shMem->semC);   //Wait for port-master to copy vessel's data to the specific port spot.
            check_semwait(checker, VESSEL);     //Check if sem_wait() function failed.
            parkingTime = shMem->vessel.parkPeriod;
            checker = sem_post(&shMem->semA);   //Port master will continue with other vessels.
            check_sempost(checker, VESSEL);     //Check if sem_post() function failed.
            sleep(parkingTime);                 //Parking time starts here for vessel.
            checker = sem_wait(&shMem->semB);   //Vessel's parking time is over. Waiting for departing approval.
            check_semwait(checker, VESSEL);     //Check if sem_wait() function failed.
            DataForPortMaster(shMem);           //Write vessel's info to shared memory. Port master will then evaluate it.
            PrintLeaveVessel(shMem);            //__PRINTER.
            /****Initial semD value is 1.***/
            checker = sem_wait(&shMem->semD);   //Only docked vessels can wait on this semaphore.
            check_semwait(checker, VESSEL);     //Check if sem_wait() function failed.
            checker = sem_post(&shMem->semA);   //Inform port-master about departure.
            check_sempost(checker, VESSEL);     //Check if sem_post() function failed.
            checker = sem_wait(&shMem->semE);   //Wait for port-master's approval for exiting maneuver.
            check_semwait(checker, VESSEL);     //Check if sem_wait() function failed.
            PrintManeuver(shMem);               //Vessel is maneuvering (exiting port).
            checker = sem_post(&shMem->semA);   //Port-master will service other vessels.
            check_sempost(checker, VESSEL);     //Check if sem_post() function failed.
        }
        else if (shMem->vessel.canDock == NO)   //Vessel cannot dock. Specified port spots are full.
        {
            PrintGoBack(shMem);                 //__PRINTER.
            //Vessel will go back to approach lane and wait for a specific port to empty.
            waitingTime = shMem->vessel.waitingTime;
            checker = sem_post(&shMem->semA);   //Port-master will service other vessels.
            check_sempost(checker, VESSEL);
            sleep(waitingTime);                 //Vessel starts waiting.
        }
    }
    checker = shmdt((void*)shMem);              //Detach shared memory.
    check_shmdt(checker, VESSEL);               //Check if shmdt() function failed.
    free(GetStrType());                         //Free malloc-ed arguments.
    free(GetStrPosType());
    free(GetStrParkPeriod());
    free(GetStrManTime());
    free(GetStrShmid());
    return 0;
}
