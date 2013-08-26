

#ifndef SPECTRUM_H_
#define SPECTRUM_H_

#include "dataMsg_m.h"

class spectrum : public cDelayChannel
{
    public:
        spectrum();
        virtual ~spectrum();
        virtual void processMessage(cMessage *msg, simtime_t t, result_t& result);
};

Define_Channel(spectrum);

#endif /* SPECTRUM_H_ */
