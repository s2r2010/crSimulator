
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * It is intended to produce PU activity patterns that match the real observed activities for GSM. In prototype state, needs development
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
    puState = "IDLE";

    if(strcmp("gsm1", getParentModule()->getName()) == 0){
        puChannel = 1;
        //idleDuration = exponential(.3);
        //busyDuration = 1 - idleDuration;
        puColor = "red";
        idleDuration = 0.0400;
        busyDuration = 0.0600;
    }
    else if (strcmp("gsm2", getParentModule()->getName()) == 0){
        puChannel = 2;
        //idleDuration = exponential(.3);
        //busyDuration = 1 - idleDuration;
        puColor = "blue";
        idleDuration = 0.6000;
        busyDuration = 0.4000;
    }
    LOG("Initialize");

    LOG("::: myChannel = "<<puChannel<< ", Arrival rate = "<<idleDuration<< ", busyDuration = "<<busyDuration);
    updateGUI(-1,-1);

    if ( status == true){
        setTimer();
    }

}

void puGSM::handleMessage(cMessage *msg)
{
    if(msg == apptimer)
    {
        puState = "Tx";
        delete msg;  apptimer = NULL;
        dataMsg *msg = new dataMsg("PUSTART");
        msg->setKind(PUSTART);
        msg->setProposedChannel(puChannel);
        msg->setSource(0);

        broadcast(msg);
        scheduleEot();
        LOG("Sending PUSTART. Starting Data transmission.");
        std::string display = "i=device/antennatower,"+puColor+",40";
        this->getParentModule()->setDisplayString(display.c_str());

        // reset duration of next comm. 60% duty cycle
        //idleDuration = exponential(0.3);
        //if(idleDuration > 1)
            //idleDuration = 1;
        //busyDuration = 1 - idleDuration;
    }
    else if (msg == eot)  // Signal end of PU transmission
    {
        puState = "IDLE";
        delete msg; eot = NULL;
        dataMsg *puEnd = new dataMsg("PUEND");
        puEnd->setKind(PUEND);
        puEnd->setProposedChannel(puChannel);
        broadcast(puEnd);

        setTimer();  // Schedule another PU transmission.
        LOG("Sending PUEND. Data transmission done.");
        this->getParentModule()->setDisplayString("i=device/antennatower");
    }
    else
    {
        //ev<<"Unknown Message at PU HandleMessage"<<endl;
        delete msg;
    }
}

void puGSM::setTimer()
{
    apptimer = new timerMsg("timer");
    scheduleAt(simTime()+ idleDuration, apptimer);
    updateGUI(simTime().dbl()+ idleDuration,puChannel);
}

void puGSM::broadcast(dataMsg *msg)
{
    for ( int x=0; x<gateSize("radio"); x++)
    {
           dataMsg *copy = (dataMsg *) msg->dup();
           send(copy, "radio$o", x);
    }
    delete msg;
}

void puGSM::scheduleEot()
{
    eot = new cMessage("PUEnd");
    scheduleAt(simTime() + busyDuration, eot);
    updateGUI(simTime().dbl()+ idleDuration,puChannel);
}

void puGSM::updateGUI(double time, int chID)
{
    // GUI changes
    std::stringstream sstr;
    sstr << "i=shah/antenna," << colorMap[chID] << ",40;t=Ch: " << chID
    << "\nNext:" << time << " State: " << puState;
    //std::string str1 = sstr.str();
    //this->getParentModule()->setDisplayString(str1.c_str());
}

