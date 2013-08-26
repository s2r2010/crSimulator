
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * It is intended to produce PU activity patterns that match the real observed activities for GSM.
 */

#include "puGSM.h"

puGSM::puGSM() {
}

puGSM::~puGSM() {
}

void puGSM::initialize()
{
    LOG("Initialize");
    apptimer = eot = NULL;
    totalChannels = getParentModule()->getParentModule()->par("totalChannels");
    bool status = getParentModule()->par("puEnabled");
    ev<<status;

    idleDuration = exponential(.4);
    busyDuration = exponential(.6);
    LOG("::: myChannel = "<<puChannel<< ", Arrival rate = "<<idleDuration<< ", TxDuration = "<<busyDuration);

}

void puGSM::handleMessage(cMessage *msg)
{
        delete msg;
}

void puGSM::setTimer()
{
     //apptimer = new timerMsg("timer");
     //scheduleAt(simTime()+ arrivalRate, apptimer);
}

void puGSM::broadcast(dataMsg *msg)
{
    //for ( int x=0; x<gateSize("radio"); x++)
    //{
           //dataMsg *copy = (dataMsg *) msg->dup();
           //send(copy, "radio$o", x);
    //}
    //delete msg;
}

void puGSM::scheduleEot()
{
    eot = new cMessage("PUEnd");
    scheduleAt(simTime()+busyDuration, eot);
}

