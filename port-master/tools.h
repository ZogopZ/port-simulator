#ifndef _TOOLS_H_
#define _TOOLS_H_
#include "../myport/port.h"

void SearchDock(sharedMemory_t *shMem, FILE* fpLog);

void SearchSmallType(sharedMemory_t* shMem,  FILE* fpLog);

void SearchMediumType(sharedMemory_t* shMem,  FILE* fpLog);

void SearchLargeType(sharedMemory_t* shMem,  FILE* fpLog);

void DockVessel(sharedMemory_t* shMem);

void UndockVessel(sharedMemory_t* shMem);

void GetWaitingTime(sharedMemory_t* shMem);

void CreateRandomVessel(void);

void SigHandler(int signo);

#endif /* _TOOLS_H_ */
