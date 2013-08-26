

#include "spectrum.h"

spectrum::spectrum()
{
}

spectrum::~spectrum()
{
}

void spectrum::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
    dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
    if(recMsg->getProposedChannel() == 1 )  // A msg on channel 1 by CR
    {
        result.delay = 0.005;
    }
    else if (recMsg->getProposedChannel() == 2){
        result.delay = 0.005;
    }
    else if (recMsg->getProposedChannel() == 3){
        result.delay = 0.005;  // 5 miliseconds
    }
}
