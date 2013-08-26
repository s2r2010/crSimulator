

#include "testDRM.h"

void testDRM::initialize()
{
    myAddress = getParentModule()->par("address");
    state = this->getParentModule()->par("drmEnabled");
    macTimer = NULL;
    if (state){
        macTimer = new timerMsg("drmch");
        scheduleAt(simTime(), macTimer);
    }
}
void testDRM::handleMessage(cMessage *msg)
{
    if (msg == macTimer){
        delete msg;
        macTimer = NULL;
        sendDrmCh();    // Send the recommended channel to MAC
    }
    else if (msg->arrivedOn("sclInterface$i")){  // msg from spectrum sensor.
        delete msg;
    }
}

void testDRM::sendDrmCh(){
    dataMsg *msg = new dataMsg("DRM");
    msg->setKind(DRMCH);
    if(strcmp(this->getParentModule()->getName(), "CR1") == 0)
        msg->setProposedChannel(3); //fixed recommendation
    else
        msg->setProposedChannel(intuniform(1,2)); // Either channel 1 or two.

    send(msg, "sclInterface$o");
    // Set another timer
    macTimer = new timerMsg("drmch");
    scheduleAt(simTime()+0.3, macTimer);
}

void testDRM::finish(){
    if(macTimer != NULL){
        cancelAndDelete(macTimer);
        macTimer = NULL;
    }
}


