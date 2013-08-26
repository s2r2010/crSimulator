
#include "puISM.h"

void puISM::initialize()
{
    bool status = getParentModule()->par("puEnabled");
    if ( status == true){
        apptimer = waitTimer = eot = NULL;
        totalChannels = getParentModule()->getParentModule()->par("totalChannels");
        arrivalRate = getParentModule()->par("arrivalRate");
        puChannel = 0;
        setTimer();
    }
}

void puISM::handleMessage(cMessage *msg)
{
    if(msg == apptimer)
    {
        delete msg;
        apptimer = NULL;
        // call a broadcast message
        dataMsg *msg = new dataMsg("PUSTART");
        msg->setKind(PUSTART);
        //puChannel = intuniform(1, totalChannels);
        puChannel = intuniform(1, 2);;
        msg->setProposedChannel(puChannel);
        broadcast(msg);
        scheduleEot();
    }
    else if (msg == eot)  // Signal end of PU transmission
    {
        delete msg; eot = NULL;
        dataMsg *puEnd = new dataMsg("PUEND");
        puEnd->setKind(PUEND);
        puEnd->setProposedChannel(puChannel);
        broadcast(puEnd);
        setTimer();  // Schedule another PU transmission.
    }
    else
    {
        delete msg;
    }
}

void puISM::setTimer()
{
     apptimer = new timerMsg("timer");
     scheduleAt(simTime()+arrivalRate, apptimer);
}

void puISM::broadcast(dataMsg *msg)
{
    // broadcast this message
    for ( int x=0; x<gateSize("radio"); x++)
    {
           dataMsg *copy = (dataMsg *) msg->dup();
           send(copy, "radio$o", x);
    }
    delete msg;
}

void puISM::scheduleEot()
{
    eot = new cMessage("PUEnd");
    scheduleAt(simTime()+par("puTxDuration"), eot);
}
