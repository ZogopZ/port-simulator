#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "../myport/port.h"
#include "../myport/cli_utils.h"
#include "tools.h"


void DataForPortMaster(sharedMemory_t* shMem)
{
    shMem->vessel.pid = getpid();                       //Data from newly arrived vessel. To be evaluated from port-master.
    shMem->vessel.type = atoi(GetStrType());
    shMem->vessel.upgradeTo = atoi(GetStrPosType());
    shMem->vessel.parkPeriod = atoi(GetStrParkPeriod());
    shMem->vessel.manTime = atoi(GetStrManTime());
    shMem->vessel.waitingTime = TIME_OUT;
    return;
}
