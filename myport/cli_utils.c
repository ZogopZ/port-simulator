#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli_utils.h"
#include "checker.h"


int ARGconfigFile;
char* configFile;

int ARGtime;
char* strTime;

int ARGstatTimes;
char* strStatTimes;

int ARGtype;
char* strType;

int ARGposType;
char* strPosType;

int ARGparkPeriod;
char* strParkPeriod;

int ARGmanTime;
char* strManTime;

int ARGshmid;
char* strShmid;

void ParseCliArgs(int argc, char** argv, int caller)
{
    int i;

    parseNcheck(argc, ARGC_CHECK, caller);      //Check correct number of arguments for current process.
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0)         //***myport*** Search arguments for flag -l.
        {
            ARGconfigFile = i + 1;              //The argument following flag -l is the configuration file.
            configFile = mallocNcheck(strlen(argv[ARGconfigFile]), caller);
            strcpy(configFile, argv[ARGconfigFile]);
        }
        else if (strcmp(argv[i], "-t") == 0)    //***vessel*** Search arguments for flag -t.
        {
            ARGtype = i + 1;                    //The argument following flag -t is type of the vessel.
            strType = mallocNcheck(strlen(argv[ARGtype]), caller);
            strcpy(strType, argv[ARGtype]);
        }
        else if (strcmp(argv[i], "-u") == 0)    //***vessel*** Search arguments for flag -u.
        {
            ARGposType = i + 1;                //The argument following flag -u is the upgrade request of the vessel.
            strPosType = mallocNcheck(strlen(argv[ARGposType]), caller);
            strcpy(strPosType, argv[ARGposType]);
        }
        else if (strcmp(argv[i], "-p") == 0)    //***vessel*** Search arguments for flag -p.
        {
            ARGparkPeriod = i + 1;              //The argument following flag -p is the park time of the vessel.
            strParkPeriod = mallocNcheck(strlen(argv[ARGparkPeriod]), caller);
            strcpy(strParkPeriod, argv[ARGparkPeriod]);
        }
        else if (strcmp(argv[i], "-m") == 0)    //***vessel*** Search arguments for flag -m.
        {
            ARGmanTime = i + 1;                 //The argument following flag -m is harboring manouver time.
            strManTime = mallocNcheck(strlen(argv[ARGmanTime]), caller);
            strcpy(strManTime, argv[ARGmanTime]);
        }
        else if (strcmp(argv[i], "-s") == 0)    //***All processes*** Search arguments for flag -s.
        {
            ARGshmid = i + 1;                   //The argument following flag -s is the key to the shared segment.
            strShmid = mallocNcheck(strlen(argv[ARGshmid]), caller);
            strcpy(strShmid, argv[ARGshmid]);
        }
    }
}

/*FUNCTIONS THAT RETURN ARGUMENTS OF PROCESSES*/
char* GetConfigFile(void)
{
    return (configFile);
}
char* GetStrTime(void)
{
    return (strTime);
}
char* GetStrType(void)
{
    return (strType);
}
char* GetStrPosType(void)
{
    return (strPosType);
}
char* GetStrParkPeriod(void)
{
    return (strParkPeriod);
}
char* GetStrManTime(void)
{
    return (strManTime);
}
char* GetStrShmid(void)
{
    return (strShmid);
}
