
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * An RTS/CTS based Mac implementation that is connected with Spectrum Sensing Module and can avoid Primary Users by switching channels. Channel switching/mobility
 * is based on the recommendation of DRM component
 */

#include "drmMacLayer.h"

void drmMacLayer::initialize()
{
    drmEnabled = par("drmEnabled");
    ackEnabled = par("AckEnabled");
    totalFrames = par("totalFrames"); // Number of packets per transmission session.
    rtsAttempts = par("rtsAttempts");
    perPacketSensing = par("sensePerPacket");
    myAddress = getParentModule()->par("address");

    numberOfPackets = currentDestination = currentDataChannel = proposedChannel = ctsDestination = 0;     // No frames to send at the time of initialization
    drmChannel = 1;
    isReceiving = isTransmitting = rtsSent = false;  // set to true when receiving/receiving data from another or when RTS is sent.

    rtsTimer = ackTimer = NULL;

    // Statistics
    rtsSignal = registerSignal("rtsSignal");
    handover = registerSignal("handover");
    nackSignal = registerSignal("nackSignal");
    dataSignal = registerSignal("dataSignal");
}
/////////////////////////////////////////////////// HANDLERS   /////////////////////////////////////////

void drmMacLayer::handleMessage(cMessage *msg)
{
    if (msg == rtsTimer)  // No response received from destination node for the sent RTS.
    {
        delete msg;
        ev<< "Node: "<<myAddress<<". No response to my sent RTS\n";
        rtsTimer = NULL;
        // ATTEMPT AGAIN
        if (rtsAttempts >= 1)
            senseRequest(SenseFreeCHANNEL);
        else{
            notifyAppLayer(0);
        }
    }
    if (msg == ackTimer)  // Ack timer fired and no response received... Send frame again. Ack timer should be scheduled only when ACks are enabled.
    {
        delete msg;
        ackTimer = NULL;
        ev<<"MAC: No ack received for sent packets.\n";
        sendData();
    }
    else if(msg->arrivedOn("ctrlUpper$i")) // ctrl msg from network layer
    {
            ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
            if (recMsg->getKind() == AppREQUEST) // A request for sending some data
            {
                if (isReceiving == true) // postpone own communication
                {
                    ev<<"Postpone my own communication due to reception.\n";
                    delete recMsg;
                }
                else  // Attempt to start communication
                {
                    currentDestination = recMsg->getDestination();
                    numberOfPackets = par("totalFrames");
                    rtsAttempts = par("rtsAttempts");
                    delete recMsg;

                    if(drmEnabled == true)
                        senseRequest(SenseDrmCHANNEL);
                    else  // DRM is not enabled. Sense any free channel
                        senseRequest(SenseFreeCHANNEL);
                }
            }
    }
    else if(msg->arrivedOn("dataLower$i")) // data msg from outside world for me....
    {
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        if (recMsg->getDestination() == myAddress)        // Message for me?
        {
            switch(recMsg->getKind())
             {
             case RTS:  handleRTS(recMsg); break;
             case CTS:  handleCTS(recMsg); break;
             case DATA:  handleData(recMsg);break;
             case ACK:  delete recMsg; handleAck(); break;
             case RTSNACK:  handleNack(RTSNACK); delete recMsg; break;
             case DATANACK: handleNack(DATANACK); delete recMsg; break;
             case EOT: handleEOT(); delete recMsg; break;
             default: ev<<"MAC: Unknown Message type\n"; delete recMsg; break;
             }
        }
        else        // Message for others.
            delete recMsg;
    }
    else if(msg->arrivedOn("ssInterface$i"))
    {
        ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
        switch(recMsg->getKind())
        {
            case SenseFreeREPLY:
                if (recMsg->getChannelID() == 0)
                {
                    ev<<"MAC: No free channel available\n";
                    delete recMsg;
                }
                else // Free channel found. If This node is not receiving, send RTS.
                {
                    if (isReceiving == false)
                    {
                        currentDataChannel = recMsg->getChannelID();
                        sendRTS(recMsg->getChannelID());
                        delete recMsg;
                    }
                    else
                    {
                        ev<<"MAC: Receiving data from another. Postponing own communication\n";
                        delete recMsg;
                    }
                }
                break;
            case SenseDataREPLY:
                if(recMsg->getChannelState() == true) // Data channel is still free.
                {
                    // Send Data/frame
                    delete recMsg;
                    sendData();
                }
                else // Data channel lost. What to do next?
                {
                    ev<<"MAC: Data channel lost. Handover needed\n";
                    delete recMsg;
                }
                break;
            case SenseProposedREPLY:
                if(recMsg->getChannelState() == true) // proposed channel is free.. Send CTS
                {
                    delete recMsg;
                    currentDataChannel = proposedChannel;
                    sendCTS();
                }
                else
                {
                    delete recMsg;
                    sendNack(RTSNACK);
                }
                break;
            case SenseDrmREPLY:
                if (recMsg->getChannelState() == true)
                {
                    if (isReceiving == false)
                    {
                        currentDataChannel = drmChannel;
                        sendRTS(drmChannel);
                    }
                    else
                        ev<<"MAC: Receiving data from another. Postponing own communication\n";
                }
                else{ // DRM channel is busy. Attempt again
                    senseRequest(SenseFreeCHANNEL);
                    //senseRequest(SenseDrmCHANNEL);
                }
                delete recMsg;
                break;
            case PUSTART:
                handlePU();
                delete recMsg;
                break;
            default:
                delete recMsg;
                ev<<"MAC: Unknown reply from SS\n";
                break;
        }
    }
    else if(msg->arrivedOn("sclInterface$i")){
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        drmChannel = recMsg->getProposedChannel();
        delete recMsg;
    }
}

void drmMacLayer::handleRTS(dataMsg *msg)
{
    if (rtsSent == true) // have sent my own RTS
    {   ev<< "Message received at node "<< myAddress << ", but I had sent my own RTS.\n";
        delete msg;
    }
    else if(isReceiving == true && msg->getSource() != ctsDestination) // already receiving data from anothter node
    {   ev<<"Node "<<myAddress<<": Already receiving data, deleting RTS\n";
        delete msg;
    }
    else if(isTransmitting == true) // already transmitting
    {   ev<<"Node "<<myAddress<<": Already transmitting data. delete RTS\n";
        delete msg;
    }
    else // I am idle, sense the proposed channel
    {
        isReceiving = true;
        this->getParentModule()->setDisplayString("i=device/palm2");
        rtsSent = false; // I am not a transmitter
        proposedChannel = msg->getProposedChannel();
        ctsDestination = msg->getSource();
        delete msg;
        senseRequest(SenseProposedCHANNEL);
    }
}
void drmMacLayer::handleData(dataMsg *msg)
{
    dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
    emit(dataSignal, 1);
    if(ackEnabled == true && currentDataChannel != 0)
    {
        dataMsg *ack = new dataMsg("ACK");
        //ack->setBitLength(300);
        ack->setKind(ACK);
        ack->setSource(myAddress);
        ack->setDestination(recMsg->getSource());
        ack->setNumberOfPackets(recMsg->getNumberOfPackets());
        ack->setProposedChannel(recMsg->getProposedChannel());
        delete recMsg;
        send(ack, "dataLower$o");
        // GUI changes
        if (currentDataChannel == 1){
            this->getParentModule()->setDisplayString("i=device/palm2,red,40");
        }
        else if (currentDataChannel == 2){
            this->getParentModule()->setDisplayString("i=device/palm2,blue,40");
        }
        else if (currentDataChannel == 3){
            this->getParentModule()->setDisplayString("i=device/palm2,green,40");
        }
    }
    else
    {
        delete recMsg;
    }
}
void drmMacLayer::handleCTS(dataMsg *msg)
{
    // Check if the proposed channel is still free
    if(currentDataChannel != 0 && msg->getProposedChannel() == currentDataChannel){
        clearRTStimer(); // clear an RTS trigger to bring node to idle state.
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        rtsSent = false;
        currentDataChannel = recMsg->getProposedChannel();
        delete recMsg;
        sendData();
    }
    else{ // proposed channel is no longer free. Sense a new channel for sending
        ev<<"Proposed channel is not free at transmitter anymore\n";
        if (rtsTimer != NULL)
        {
            cancelAndDelete(rtsTimer); rtsTimer = NULL;
        }
        delete msg;
        rtsAttempts = par("rtsAttempts");
        senseRequest(SenseFreeCHANNEL);
    }
}
void drmMacLayer::handleAck()
{
    // clear ACK timeout timer
    if(ackTimer != NULL)
    {
        cancelAndDelete(ackTimer);    ackTimer = NULL;
    }
    numberOfPackets--;
    if (numberOfPackets != 0){  // More data to send
        if (currentDataChannel == 0) // channel is lost. Dont send next packet
        {  ev<<"MAC: Data channel is lost\n";  }
        else
        {
            // if sense per packet is disabled, send next packet.. otherwise sense the data channel
            if (perPacketSensing == true)
            {
                // send data channel sense request
                senseRequest(SenseDataCHANNEL);
            }
            else
            {
                sendData();  // Send the next packet.
            }
        }
    }
    else{ // No more data to send. Notify App Layer of Success. And send EOT and Get Idle
        sendEOT();
        ev<<"MAC: All frames have been sent\n";
        ctrlMsg *txFinish = new ctrlMsg("Tx Success");  // Inform Application layer about successful transmission of data.
        txFinish->setKind(TxSUCCESS);
        send(txFinish, "ctrlUpper$o");
        getIdle();  // Come to an idle MAC state.
    }
}
void drmMacLayer::handleNack(int type)
{
    emit(nackSignal, 1);
    if (isTransmitting == true){
        switch(type)
        {
        case RTSNACK:
            ev<<"Node "<<myAddress<<": NACK received, sending RTS on another channel\n";
            rtsSent=false;
            rtsAttempts = par("rtsAttempts");
            clearRTStimer();
            senseRequest(SenseFreeCHANNEL);
            break;
        case DATANACK: // implies a change of channel and not transmission related loss of data
            ev<<"Node "<<myAddress<<": NACK received, sending Frame on another channel\n";
            rtsSent=false;
            this->getParentModule()->setDisplayString("i=device/palm2");
            clearAckTimer();
            senseRequest(SenseFreeCHANNEL);
            break;
        default:
            break;
        }
    }
    else if (isReceiving == true){
        getIdle();
    }
    else
        ev<<"MAC: Problem in handleNack()\n;";
}
void drmMacLayer::handleEOT()
{
    getIdle();
    this->getParentModule()->setDisplayString("i=device/palm2");
    dataMsg *eot = new dataMsg("EOT");
    eot->setKind(EOT); // No destination specified so that others can ignore it at mac layer
    send(eot, "dataLower$o");
}
void drmMacLayer::handlePU()
{
    if(isTransmitting == true){  // I was transmitting
        emit(handover, 1);
        if(ackEnabled == true){
            // cancel the ack timer, start RTS/CTS for a new channel.
            if (ackTimer != NULL){ cancelAndDelete(ackTimer); ackTimer = NULL;}
        }
        currentDataChannel = 0;
        this->getParentModule()->setDisplayString("i=device/palm2");
        rtsAttempts = par("rtsAttempts");
        if(rtsSent != true){
            if(drmEnabled == true)
                senseRequest(SenseDrmCHANNEL);
            else
                senseRequest(SenseFreeCHANNEL);
        }
    }
    else if(isReceiving == true) {  // I was receiving. Inform Tx if it is not affected by this PU
        emit(handover, 1);
        sendNack(DATANACK);
        getIdle();
    }
    else { // Was neither tx nor rx. May be I only send RTS at this point.
        ev<<"MAC: PU during RTS/CTS. Getting into idle state\n";
        getIdle();
    }
}
////////////////////////////////////////////////// SEND FUNCTIONS ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void drmMacLayer::sendRTS(int proposedChannel) // send RTS on a free channel, from spectrum sensing.
{
    isTransmitting = true;
    if (rtsAttempts >= 1)
    {
        emit(rtsSignal, 1);
        dataMsg *rts = new dataMsg("RTS");
        rts->setSource(myAddress);
        rts->setDestination(currentDestination);
        rts->setProposedChannel(proposedChannel);
        rts->setKind(RTS);
        rts->setByteLength(65000);

        ev<<"Node "<<myAddress<<": RTS to N-"<<rts->getDestination()<<" for channel "<<rts->getProposedChannel()<<". RTS attempt # "<< rtsAttempts <<"\n";
        send(rts, "dataLower$o");
        rtsAttempts--;
        rtsSent = true;
        setRTStimer();  // If no response is received, send another RTS if multiple RTS are enabled.
    }
    else // All rts attempts failed
    {
        getIdle();  notifyAppLayer(0);
    }
}
void drmMacLayer::sendCTS()
{
   dataMsg *cts = new dataMsg("CTS");
   cts->setKind(CTS);
   cts->setSource(myAddress);
   cts->setDestination(ctsDestination);
   cts->setProposedChannel(currentDataChannel);
   ev<<"Node "<<myAddress<<": Sending CTS to "<<cts->getDestination()<<".\n";
   send(cts, "dataLower$o");
}
void drmMacLayer::sendData()
{
    if (currentDataChannel == 1){
        this->getParentModule()->setDisplayString("i=device/palm2,red,40");
    }
    else if (currentDataChannel == 2){
        this->getParentModule()->setDisplayString("i=device/palm2,blue,40");
    }
    else if (currentDataChannel == 3){
        this->getParentModule()->setDisplayString("i=device/palm2,green,40");
    }

    if (ackEnabled == true) // Send the next packet only after receiving ACK
    {
       if(currentDataChannel !=0)  // Frame sending retries....
       {
           dataMsg *frame = new dataMsg("Data");
           frame->setSource(myAddress);
           frame->setKind(DATA);
           frame->setDestination(currentDestination);
           frame->setNumberOfPackets(numberOfPackets);
           frame->setProposedChannel(currentDataChannel);
           send(frame, "dataLower$o");
           setAckTimeOut();
       }
       else
           getIdle();
    }
}
void drmMacLayer::sendNack(int type)
{
    //isReceiving = false;
    dataMsg *nack = new dataMsg("Nack");
    switch(type)
    {
    case RTSNACK:
        nack->setKind(RTSNACK);
        break;
    case DATANACK:
        nack->setKind(DATANACK);
        break;
    default:
        break;
    }
     nack->setDestination(ctsDestination);
     send(nack, "dataLower$o");
}
void drmMacLayer::sendEOT(){
    // send End of Transmission Message
    dataMsg *eot = new dataMsg("EOT");
    eot->setSource(myAddress); eot->setDestination(currentDestination);
    eot->setProposedChannel(currentDataChannel);
    eot->setKind(EOT);
    send(eot, "dataLower$o");
}

////////////////////////////////////////////// UTILITY FUNCTIONS ////////////////////////////////////////
void drmMacLayer::senseRequest(int type)
{
    ctrlMsg *senseRequest = new ctrlMsg("SenseRequest");
    switch (type)
    {
        case SenseFreeCHANNEL:  // sense any free channel request
            senseRequest->setKind(SenseFreeCHANNEL);
            break;
        case SenseDataCHANNEL:  // sense the data channel request
            senseRequest->setKind(SenseDataCHANNEL);
            senseRequest->setChannelID(currentDataChannel);
            break;
        case SenseProposedCHANNEL:  // sense the channel proposed by Tx
            senseRequest->setKind(SenseProposedCHANNEL);
            senseRequest->setChannelID(proposedChannel);
            break;
        case SenseDrmCHANNEL:
            senseRequest->setKind(SenseDrmCHANNEL);
            senseRequest->setChannelID(drmChannel);
            break;
        default:
            ev<< "MAC: Unknown sensing request\n";
            break;
    }
    send(senseRequest, "ssInterface$o");
}
void drmMacLayer::getIdle()
{
    this->getParentModule()->setDisplayString("i=device/palm2");
    if (rtsTimer != NULL)
    {
        cancelAndDelete(rtsTimer); rtsTimer = NULL;
    }
    if (ackTimer != NULL){
        cancelAndDelete(ackTimer); ackTimer = NULL;
    }
    rtsAttempts = currentDataChannel = 0;
    isReceiving = isTransmitting = rtsSent = false;
    rtsTimer = ackTimer = NULL;
}

/////////////////////////////////////////////////// TIMERS  //////////////////////////////////////////
void drmMacLayer::setRTStimer()  // is called when RTS is sent.
{
    if (rtsTimer == NULL)
    {
        ev<<"RTS Timeout timer set\n";        rtsTimer = new timerMsg("RTS-timer");        scheduleAt(simTime()+ 0.3, rtsTimer);
    }
}
void drmMacLayer::clearRTStimer()
{
    if(rtsTimer != NULL)
    {
        ev<<"RTS timer cleared\n";        cancelAndDelete(rtsTimer);        rtsTimer = NULL;
    }
}
void drmMacLayer::clearAckTimer()
{
    if(ackTimer != NULL)
    {
        ev<<"AcK timer cleared\n";        cancelAndDelete(ackTimer);        ackTimer = NULL;
    }
}
void drmMacLayer::setAckTimeOut()
{
    if (ackTimer == NULL)
    {
        ackTimer = new timerMsg("Ack-timeout");        scheduleAt(simTime()+0.12, ackTimer);
    }
}
 //////////////////////////////////////////////////////////////////
void drmMacLayer::notifyAppLayer(int type){
    ctrlMsg *txStatus = new ctrlMsg("TxFail");
    switch(type){
    case 0:  //Tx fail
        txStatus->setKind(TxFAIL);
        break;
    case 1:  // Tx Success
        txStatus->setKind(TxSUCCESS);
        break;
    default:
        break;
    }
    send(txStatus, "ctrlUpper$o");
}
void drmMacLayer::finish(){
    getIdle();
}











