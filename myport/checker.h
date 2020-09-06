#ifndef _CHECKER_H_
#define _CHECKER_H_

#include <stdio.h>

#include "port.h"

#define ARGC_CHECK      -109
#define MYPORT          -108
#define PORT_MASTER     -107
#define VESSEL          -106
#define VESSEL_CREATOR  -105
#define VALID           -104
#define INVALID         -103
#define CONFIG_FILE     -102
#define LOG_FILE        -101

#define ARGS_MYPORT 3
#define ARGS_PORT_MASTER 3
#define ARGS_VESSEL 11


/**************** SHARED MEMORY CHECKER *****************/
void check_shmget(int checker, int caller);
void check_shmat(sharedMemory_t* checker, int caller);
void check_shmctl(int checker, int caller);
void check_shmdt(int checker, int caller);

/***************** SEMAPHORE CHECKER ********************/
void check_seminit(int checker, int caller);
void check_semdestroy(int checker, int caller);
void check_semwait(int checker, int caller);
void check_semget(int checker, int caller);
void check_sempost(int checker, int caller);

/******************* OTHER CHECKERS *********************/
void* mallocNcheck(size_t incomingSize, int caller);
void check_fork(pid_t pidChecker, int caller);
void check_fopen(FILE* fpChecker , int whichFile, int caller);
void check_fclose(int checker, int whichFile, int caller);
void check_exec(int caller);
void parseNcheck(int fpChecker, int function, int caller);
void check_wait(int checker, int caller);

#endif /* _CHECKER_H_ */
