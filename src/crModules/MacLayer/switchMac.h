
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * An empty box for implementation of SWITCH protocol which is a CR specific Mac protocol.
 */

#ifndef SWITCHMAC_H_
#define SWITCHMAC_H_
#include "BaseMacLayer.h"
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "timerMsg_m.h"
class switchMac : public BaseMacLayer{
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(switchMac);

#endif /* SWITCHMAC_H_ */
