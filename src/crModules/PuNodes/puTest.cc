/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * A test implementation. Only for a specific topology of "drmNetwork" Three PUs having independent channels, arrival rates and duty cycles
 */

#include "puTest.h"

void puTest::initialize()
{
    LOG("Initialize");
    apptimer = eot = NULL;
    totalChannels = getParentModule()->getParentModule()->par("totalChannels");
    bool status = getParentModule()->par("puEnabled");
    puState = "IDLE";

    //Color Map
    colorMap[0] = "black";
    colorMap[1] = "red";
    colorMap[2] = "blue";
    colorMap[3] = "green";

    if(strcmp("pu1", getParentModule()->getName()) == 0){
        puChannel = 1;
    }
    else if (strcmp("pu2", getParentModule()->getName()) == 0){
        puChannel = 2;
    }
    else if (strcmp("pu3", getParentModule()->getName())== 0){
        puChannel = 3;
    }

    arrivalRate = 0.5;
    txDuration = 0.5;

    if (puChannel > 0)
    {
        arrivalRate *= puChannel + intuniform(0,2);
        txDuration *= puChannel + intuniform(0,2);
    }

    LOG("::: myChannel = "<<puChannel<< ", Arrival rate = "<<arrivalRate<< ", txDuration = "<<txDuration);
    //updateGUI(-1,-1);

    if ( status == true){
        setTimer();
    }
}

void puTest::handleMessage(cMessage *msg)
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
    }
    else
    {
        delete msg;
    }
}

void puTest::setTimer()
{
     apptimer = new timerMsg("timer");
     scheduleAt(simTime()+ arrivalRate, apptimer);
     updateGUI(simTime().dbl() + arrivalRate,puChannel);
}

void puTest::broadcast(dataMsg *msg)
{
    for ( int x=0; x<gateSize("radio"); x++)
    {
           dataMsg *copy = (dataMsg *) msg->dup();
           send(copy, "radio$o", x);
    }
    delete msg;
}

void puTest::scheduleEot()
{
    eot = new cMessage("PUEnd");
    scheduleAt(simTime()+txDuration, eot);
    updateGUI(simTime().dbl()+ arrivalRate,puChannel);
}

void puTest::updateGUI(double time, int chID)
{
    // GUI changes
    std::stringstream sstr;
    sstr << "i=crSimulator/antenna," << colorMap[chID] << ",40;t=Ch: " << chID
    << "\nNext:" << time << " State: " << puState;
    std::string str1 = sstr.str();
    this->getParentModule()->setDisplayString(str1.c_str());
}
