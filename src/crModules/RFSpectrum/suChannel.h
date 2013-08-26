

#ifndef SUCHANNEL_H_
#define SUCHANNEL_H_

#include "dataMsg_m.h"

class suChannel : public cDelayChannel{
    protected:
        bool isTransmissionChannel() const;
        simtime_t getTransmissionFinishTime() const;
        void processMessage(cMessage *msg, simtime_t t, result_t& result);
};

Define_Channel(suChannel);


#endif /* SUCHANNEL_H_ */
