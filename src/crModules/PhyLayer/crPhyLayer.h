
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * The Phy layer receives messages from all connections/channels. Any physical layer parameters need to be appended to the outgoing messages.
 */

#ifndef CRPHYLAYER_H_
#define CRPHYLAYER_H_


#include<omnetpp.h>
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "timerMsg_m.h"

//#define totalChannels 1      // Number of channels available for users...

class crPhyLayer : public cSimpleModule {
private:
    int myAddress;
    //simsignal_t sensingSignal;
    //simsignal_t handoverSignal;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void broadcast(dataMsg *msg);
};

Define_Module(crPhyLayer);

#endif /* PHYLAYER_H_ */
