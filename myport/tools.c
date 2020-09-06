#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "tools.h"
#include "checker.h"
#include "cli_utils.h"
#include "../port-master/printer.h"

fileData_t parseConfigFile(FILE* fpLog)
{
    int checker = 0;
    char* configLine;
    char* configWord;
    char* token;
    FILE* fpConfig;
    fileData_t data;

    data.typeSmall = INVALID;
    data.typeMedium = INVALID;
    data.typeLarge = INVALID;

    configLine = mallocNcheck(14, MYPORT);      //Malloc max line size of configuration file. "-capacity2 15\n"
    configWord = mallocNcheck(10, MYPORT);      //Malloc max word size of configuration file. "-capacity1"
    token = configWord;                         //Use token pointer for strtok() calls, so we do not lose the correct configWord pointer.
    fpConfig = fopen(GetConfigFile(), "r");     //Open configuration file for reading.
    check_fopen(fpConfig, CONFIG_FILE, MYPORT); //Check if fopen() function succeeded.
    while (fgets(configLine, 14, fpConfig)) //Get a line from configuration file.
    {
        token = strtok(configLine, " ");    //Get the first token with " " (one space) as deliminator.
        if (strcmp(token, "-type1") == 0)
        {
            data.typeSmall = VALID;         //Small type parking spots are included in port creation.
            token = strtok(NULL, "\n");     //Get the second token with "\n" (newline) as deliminator.
        }
        else if (strcmp(token, "-type2") == 0)
        {
            data.typeMedium = VALID;        //Medium type parking spots are included in port creation.
            token = strtok(NULL, "\n");
        }
        else if (strcmp(token, "-type3") == 0)
        {
            data.typeLarge = VALID;         //Large type parking spots are included in port creation.
            token = strtok(NULL, "\n");
        }
        else if (strcmp(token, "-capacity1") == 0)
        {
            token = strtok(NULL, "\n");
            data.smallCap = atoi(token);    //Store the number of parking spots for small type vessels.
        }
        else if (strcmp(token, "-capacity2") == 0)
        {
            token = strtok(NULL, "\n");
            data.mediumCap = atoi(token);   //Store the number of parking spots for medium type vessels.
        }
        else if (strcmp(token, "-capacity3") == 0)
        {
            token = strtok(NULL, "\n");
            data.largeCap = atoi(token);    //Store the number of parking spots for large type vessels.
        }
        else if (strcmp(token, "-cost1") == 0)
        {
            token = strtok(NULL, "\n");
            data.smallCost = atoi(token);   //Store the price for small type parking spots.
        }
        else if (strcmp(token, "-cost2") == 0)
        {
            token = strtok(NULL, "\n");
            data.mediumCost = atoi(token);  //Store the price for medium type parking spots.
        }
        else if (strcmp(token, "-cost3") == 0)
        {
            token = strtok(NULL, "\n");
            data.largeCost = atoi(token);   //Store the price for large type parking spots.
        }
        else if (strcmp(token, "-per") == 0)
        {
            token = strtok(NULL, " ");
            data.timePeriod = atoi(token);
            token = strtok(NULL, "\n");
            if (strcmp(token, "minutes") == 0)
                data.timeType = MINUTES;
            else if (strcmp(token, "hours") == 0)
                data.timeType = HOURS;
        }
    }
    //Store the full capacity of the port. (capacity1 + capacity2 + capacity3).
    if (data.typeSmall == VALID && data.typeMedium == VALID && data.typeLarge == VALID)
        data.portCapacity = data.smallCap + data.mediumCap + data.largeCap;
    else
    {
        PrintErrorMyport(fpLog);
        exit(EXIT_FAILURE);
    }
    checker = fclose(fpConfig);
    check_fclose(checker, CONFIG_FILE, MYPORT);
    free(configWord);
    free(configLine);
    return data;
}


