#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/types.h>

#include "checker.h"
#include "cli_utils.h"
#include "tools.h"
#include "port.h"
#include "../port-master/printer.h"

int main(int argc, char** argv)
{
    FILE* fpLog = fopen("logFile", "wa+");  //Open logFile for reading and writing and appending. Create it if necessary.
    check_fopen(fpLog, LOG_FILE, MYPORT);   //Check if fopen() function failed.
    PrintMyPortInit(fpLog);                 //___PRINTER.
    int status = 0, checker = 0, shmid = 0, capacity = 0;
    fileData_t data;
    sharedMemory_t* initMem;
    sharedMemory_t* shMem;

    ParseCliArgs(argc, argv, MYPORT);       //Check in line parameters.
    data = parseConfigFile(fpLog);          //Parse configuration file and extract data for port creation.
    capacity = data.portCapacity;           //Get the capacity of the port. (sum of all types of parking spots).
    /***Struct hack.***/
    int shMemSize = sizeof(sharedMemory_t) + capacity*sizeof(portSpot_t);
    /*Malloc memory for the size of struct sharedMemory_t plus memory for each portSpot_t struct.
    Number of portSpot_t structs is defined by the total vessel capacity of the port.*/
    /******************/
    initMem = mallocNcheck(shMemSize, MYPORT);

    PortCreation(data, initMem);            //Populate struct initMem with data extracted from configuration file.
    shmid = shmget(IPC_PRIVATE, shMemSize, 0666);
    check_shmget(shmid, MYPORT);                        //Check if shmget() function failed.
    shMem = (sharedMemory_t*)shmat(shmid, (void*)0, 0); //Attach shared memory to process.
    check_shmat(shMem, MYPORT);                         //Check if shmat() function failed.
    memcpy(shMem, initMem, shMemSize);                  //Copy local memory to shared memory.
    shMem->shmid = shmid;
    free(initMem);                          //Free memory used for port initialization.
    PrintPortCreate(fpLog);                 //___PRINTER.

    char* strShmid = mallocNcheck(CountDigits(shmid), MYPORT);  //Count digits of shared memory id and malloc that many bytes.
    sprintf(strShmid, "%d", shmid);                             //Write the shared memory id to a string named str_shmid, for execl usage.
    pid_t pid = fork();                     //Fork to port-master.
    if (pid == 0)                           //Port-master child process.
    {
        execl("./port-master/port-master", "", "-s", strShmid, (char*) NULL);
        check_exec(MYPORT);                 //Check if execl() function failed.
    }
    else
    {
        check_fork(pid, MYPORT);            //Check if fork() function failed.
        PrintPortMasterCreate(fpLog);       //___PRINTER.
        checker = fclose(fpLog);            //Check if fclose() function failed.
        check_fclose(checker, LOG_FILE, MYPORT);
        checker = wait(&status);            //Wait for child port-master to finish.
        check_wait(checker, MYPORT);        //Check if wait() function failed.

    }
    fpLog = fopen("logFile", "a");
    check_fopen(fpLog, LOG_FILE, MYPORT);
    DestroySemaphores(shMem);               //Destroy semaphores.
    PrintDestroySems(fpLog);                //__PRINTER.
    checker = shmdt((void*)shMem);          //Detach process from shared memory.
    check_shmdt(checker, MYPORT);           //Check if shmdt() function failed.
    checker = shmctl(shmid, IPC_RMID, 0);   //Mark the shared segment to be destroyed.
    check_shmctl(checker, MYPORT);          //Check if shmctl() function failed.
    PrintMarkShmem(fpLog);                  //__PRINTER.
    free(strShmid);
    free(GetConfigFile());
    free(GetStrShmid());
    checker = fclose(fpLog);
    check_fclose(checker, LOG_FILE, MYPORT);//Check if fclose() function failed.
    return 0;
}

