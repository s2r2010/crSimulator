

#include "crStats.h"

void crStats::initialize()
{
    appRequests = registerSignal("appRequests");
    //emit(appRequests, 4);
}

