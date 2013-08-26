

#ifndef BASEPUNODE_H_
#define BASEPUNODE_H_

#include<omnetpp.h>
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "timerMsg_m.h"

class BasePuNode : public cSimpleModule {
public:
        BasePuNode();
        virtual ~BasePuNode();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(BasePuNode);

#endif /* PUNODE_H_ */
