#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include "port.h"
#include "checker.h"


/**************** SHARED MEMORY CHECKER *****************/
void check_shmget(int checker, int caller)
{
    if (caller == MYPORT && checker == -1)                      //shmget() function failed.
    {
        perror("---|SHARED MEMORY CREATION FAILURE MY-PORT| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_shmat(sharedMemory_t* checker, int caller)
{
    if (caller == MYPORT && *(int*)checker == -1)               //shmat() function failed.
    {
        perror("---|ATTACH FAILURE MY-PORT| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == PORT_MASTER && *(int*)checker == -1)
    {
        perror("---|ATTACH FAILURE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == VESSEL && *(int*)checker == -1)
    {
        perror("---|ATTACH FAILURE VESSEL| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_shmctl(int checker, int caller)
{
    if (caller == MYPORT && checker == -1)                      //shmctl() function failed.
    {
        perror("---|SHMCTL FAILURE MY-PORT| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_shmdt(int checker, int caller)
{
    if (caller == MYPORT && checker == -1)                      //shmdt() function failed.
    {
        perror("---|DETACH FAILURE MY-PORT| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == PORT_MASTER && checker == -1)
    {
        perror("---|DETACH FAILURE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == VESSEL && checker == -1)
    {
        perror("---|DETACH FAILURE VESSEL| ");
        exit(EXIT_FAILURE);
    }
    return;
}

/***************** SEMAPHORE CHECKER ********************/
void check_seminit(int checker, int caller)
{
    if (caller == MYPORT && checker == -1)                      //sem_init() function failed.
    {
        perror("---|SEMAPHORE INITIALIZATION FAILURE MY-PORT| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_semdestroy(int checker, int caller)
{
    if (caller == MYPORT && checker == -1)                      //sem_destroy() function failed.
    {
        perror("---|SEMAPHORE DESTROY FAILURE MY-PORT| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_semwait(int checker, int caller)
{
    if (caller == PORT_MASTER && checker == -1)                 //sem_wait() function failed.
    {
        perror("---|SEMAPHORE WAIT FAILURE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == VESSEL && checker == -1)
    {
        perror("---|SEMAPHORE WAIT FAILURE VESSEL| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_semget(int checker, int caller)
{
    if (caller == PORT_MASTER && checker == -1)                 //sem_get() function failed.
    {
        perror("---|SEMAPHORE GET-VALUE FAILURE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == VESSEL && checker == -1)
    {
        perror("---|SEMAPHORE GET-VALUE FAILURE VESSEL| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_sempost(int checker, int caller)
{
    if (caller == PORT_MASTER && checker == -1)                 //sem_post() function failed.
    {
        perror("---|SEMAPHORE POST FAILURE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == VESSEL && checker == -1)
    {
        perror("---|SEMAPHORE POST FAILURE VESSEL| ");
        exit(EXIT_FAILURE);
    }
    return;
}

/******************* OTHER CHECKERS *********************/
void* mallocNcheck(size_t incomingSize, int caller)
{
    void *ptr;

    ptr = malloc(incomingSize + 1);                            //+1 for terminating character.
    if (ptr == NULL && incomingSize > 0)                       //malloc() function failed.
    {
        if (caller == MYPORT)
            perror("---|MALLOC FAILURE MY-PORT| ");
        else if (caller == PORT_MASTER)
            perror("---|MALLOC FAILURE PORT_MASTER| ");
        else if (caller == VESSEL)
            perror("---|MALLOC FAILURE VESSEL");
        exit(EXIT_FAILURE);
    }
    memset(ptr, 0, incomingSize + 1);                          //Every allocated string will have a null terminating character.
    return ptr;                                                //Every pointer is initialized to NULL.
}

void check_fork(pid_t pidChecker, int caller)
{
    if (pidChecker == -1)                                       //fork() function failed.
    {
        if (caller == MYPORT)
            perror("---|FORK FAILURE MY-PORT| ");
        else if (caller == PORT_MASTER)
            perror("---|FORK FAILURE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_fopen(FILE* fpChecker, int whichFile, int caller)
{
    if ((whichFile == LOG_FILE) && (fpChecker == NULL))         //fopen() function failed for log file.
    {
        if (caller == MYPORT)
            perror("---|FOPEN FAILURE LOG-FILE MY-PORT| ");
        else if (caller == PORT_MASTER)
            perror("---|FOPEN FAILURE LOG-FILE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    else if ((whichFile == CONFIG_FILE) && (fpChecker == NULL)) //fopen() function failed for configuration file.
    {
        if (caller == MYPORT)
            perror("---|FOPEN FAILURE CONFIG-FILE MY-PORT| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_fclose(int checker, int whichFile, int caller)
{
    if((whichFile == LOG_FILE) && (checker == EOF))
    {
        if (caller == MYPORT)
            perror("---|FCLOSE FAILURE LOG-FILE MY-PORT| ");
        else if (caller == PORT_MASTER)
            perror("---|FCLOSE FAILURE LOG-FILE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    return;
}

void check_exec(int caller)
{
    if (caller == MYPORT)
        perror("---|EXEC FAILURE MY-PORT| ");
    else if (caller == PORT_MASTER)
        perror("---|EXEC FAILURE PORT-MASTER| ");
    exit(EXIT_FAILURE);
    return;                                                     //exec() function failed, because it returned to the caller.
}

void parseNcheck(int fpChecker, int function, int caller)       //Check correct number of arguments for each process type.
{                                                               //Myport Port-master or Vessels.
    if (caller == MYPORT)
    {
        if ((function == ARGC_CHECK) && (fpChecker != ARGS_MYPORT))
        { //Check if given arguments for myport process are exactly 3.
            printf("---|PARSE FAILURE MYPORT: Number of arguments is incorrect. Aborting...|\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (caller == PORT_MASTER)                             //+1(empty argument) for valgrind usage.
    {                                                           //-1 argument for execl() which doesn't count the executable.
        if ((function == ARGC_CHECK) && (fpChecker != ARGS_PORT_MASTER))
        { //Check if given arguments for port-master process are exactly 5.
            printf("---|PARSE FAILURE PORT-MASTER: Number of arguments is incorrect. Aborting...|\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (caller == VESSEL)                                  //+1(empty argument) for valgrind usage.
    {                                                           //-1 argument for execl() which doesn't count the executable.
        if ((function == ARGC_CHECK) && (fpChecker != ARGS_VESSEL))
        { //Check if given arguments for vessel process are exactly 11.
            printf("---|PARSE FAILURE VESSEL: Number of arguments is incorrect. Aborting...|\n");
            exit(EXIT_FAILURE);
        }
    }
    return;
}

void check_wait(int checker, int caller)
{
    if (caller == MYPORT && checker == -1)
    {
        perror("---|WAIT FAILURE MY-PORT| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == MYPORT && checker == -1)
    {
        perror("---|WAIT FAILURE PORT-MASTER| ");
        exit(EXIT_FAILURE);
    }
    else if (caller == VESSEL_CREATOR && checker == -1)
    {
        perror("---|WAIT FAILURE VESSEL-CREATOR| ");
        exit(EXIT_FAILURE);
    }
    return;
}
