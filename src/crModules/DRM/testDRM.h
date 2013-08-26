

#ifndef TESTDRM_H_
#define TESTDRM_H_

#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "timerMsg_m.h"

class testDRM : public cSimpleModule{
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    void sendDrmCh();

    int myAddress;
    bool state;         //Whether DRM is active or not
    timerMsg *macTimer;
};

Define_Module(testDRM);

#endif /* TESTDRM_H_ */
