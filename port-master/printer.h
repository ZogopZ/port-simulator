#ifndef _PRINTER_H_
#define _PRINTER_H_

char* getLogTime(void);

/*************** SIGNAL HANDLER PRINTER *****************/
void PrintSigusr1(void);
void PrintChangeSpeed(void);

/********************MY-PORT PRINTER ********************/
void PrintMyPortInit(FILE* fpLog);
void PrintPortCreate(FILE* fpLog);
void PrintPortMasterCreate(FILE* fpLog);
void PrintDestroySems(FILE* fpLog);
void PrintMarkShmem(FILE* fpLog);
void PrintErrorMyport(FILE* fpLog);

/***************** PORT-MASTER PRINTER ******************/
void PrintPort(sharedMemory_t* shMem, FILE* fpLog);
void PrintPortMasterInit(FILE* fpLog);
void PrintVesselCreator(void);
void PrintPortClosing(void);
void PrintRequest(sharedMemory_t* shMem, FILE* fpLog);
void PrintFullPort(sharedMemory_t* shMem, FILE* fpLog);
void PrintFullType(int type, sharedMemory_t* shMem, FILE* fpLog);
void PrintPortType(sharedMemory_t* shMem, FILE* fpLog);
void PrintSuccessDock(sharedMemory_t* shMem, FILE* fpLog);
void PrintUndock(sharedMemory_t* shMem, FILE* fpLog);
void PrintVesselWait(sharedMemory_t* shMem, FILE* fpLog);
void PrintLeavePortMaster(sharedMemory_t* shMem, FILE* fpLog);

/******************** VESSEL PRINTER ********************/
void PrintGoBack(sharedMemory_t* shMem);
void PrintWillToDock(sharedMemory_t* shMem);
void PrintManeuver(sharedMemory_t* shMem);
void PrintLeaveVessel(sharedMemory_t* shMem);
void PrintBye(sharedMemory_t* shMem);

#endif /* _PRINTER_H_ */
