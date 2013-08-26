
/*
 * rainer.schuth@tu-ilmenau.de
 *
 * Based on the implementation from
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * An RTS/CTS based Mac implementation that is connected with Spectrum Sensing Module and can avoid Primary Users by switching channels. Channel switching/mobility
 * is based on the recommendation of DRM component
 */

#include "MobilityMacLayer.h"

void MobilityMacLayer::initialize()
{
    setCurrentState(IDLE);
    drmEnabled = par("drmEnabled");
    ackEnabled = par("AckEnabled");
    totalFrames = par("totalFrames"); // Number of packets per transmission session.
    rtsAttempts = par("rtsAttempts");
    perPacketSensing = par("sensePerPacket");
    myAddress = getParentModule()->par("address");
    broadcastAddress = getParentModule()->par("broadcastAddress");

    numberOfPackets = currentDestination = proposedChannel = ctsDestination = 0;     // No frames to send at the time of initialization
    setCurrentDataChannel(0);

    drmChannel = 1;
    isReceiving = isTransmitting = rtsSent = false;  // set to true when receiving/receiving data from another or when RTS is sent.

    rtsTimer = ackTimer = NULL;

    // Statistics
    rtsSignal = registerSignal("rtsSignal");
    handover = registerSignal("handover");
    nackSignal = registerSignal("nackSignal");
    txDataSignal = registerSignal("txDataSignal");
    txAckSignal = registerSignal("txAckSignal");
    rxDataSignal = registerSignal("rxDataSignal");
    rxAckSignal = registerSignal("rxAckSignal");
    txMobilitySignal = registerSignal("txMobilitySignal");
    rxMobilitySignal = registerSignal("rxMobilitySignal");
    totalTxPacketsSignal = registerSignal("totalTxPacketsSignal");
    totalRxPacketsSignal = registerSignal("totalRxPacketsSignal");

    //Color Map
    colorMap[0] = "black";
    colorMap[1] = "red";
    colorMap[2] = "blue";
    colorMap[3] = "green";

    rxSeqNo = -1;
    txSeqNo = 0;
    txDataQueue = new cPacketQueue();
    rxDataQueue = new cPacketQueue();
    txMobilityQueue = new cPacketQueue();
    rxMobilityQueue = new cPacketQueue();

    LOG("TEST 1");
}
/////////////////////////////////////////////////// HANDLERS   /////////////////////////////////////////

void MobilityMacLayer::handleMessage(cMessage *msg)
{
    if (msg == rtsTimer)  // No response received from destination node for the sent RTS.
    {
        delete msg;
        LOG("Node: "<<myAddress<<". No response to my sent RTS");
        rtsTimer = NULL;
        // ATTEMPT AGAIN
        if (rtsAttempts >= 1)
            senseRequest(SenseFreeCHANNEL);
        else{
            notifyAppLayer(0);
        }
    }
    else if (msg == ackTimer)  // Ack timer fired and no response received... Send frame again. Ack timer should be scheduled only when ACks are enabled.
    {
        delete msg;
        ackTimer = NULL;
        LOG("MobilityMAC: No ack received for sent packet. Retransmit");
        sendData(MACDATA);
    }
    else if(msg->arrivedOn("ctrlUpper$i")) // ctrl msg from network layer
    {
            ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
            switch(recMsg->getKind())
            {
                case AppREQUEST:    // A request for sending some data
                    handleAppREQUEST(recMsg);
                    break;
                default:
                    LOG("MobilityMacLayer-" << getIndex() << ": Unknown Message type");
                    break;
            }
            delete recMsg;
    }
    else if (msg->arrivedOn("dataUpper$i"))
    {
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        switch(recMsg->getKind())
        {
            case DATA:
                handleDataFromAbove(recMsg);
                break;
            //case ACK:
                //
            default:
                LOG("MobilityMAC: Unknown Message type");
                delete recMsg;
                break;
        }
    }
    else if(msg->arrivedOn("dataLower$i")) // data msg from outside world for me....
    {
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        if (recMsg->getDestination() == myAddress || recMsg->getDestination() == broadcastAddress )        // Message for me?
        {
            switch(recMsg->getKind())
            {
                case RTS:
                    handleRTS(recMsg);
                    break;
                case CTS:
                    handleCTS(recMsg);
                    break;
                case DATA:
                case MACDATA:
                    handleDataFromBelow(recMsg);
                    break;
                case ACK:
                case MACACK:
                    handleAck(recMsg);
                    break;
                case RTSNACK:
                    handleNack(RTSNACK);
                    delete recMsg;
                    break;
                case DATANACK:
                    handleNack(DATANACK);
                    delete recMsg;
                    break;
                case EOT:
                    handleEOT();
                    delete recMsg;
                    break;
                case MOBILITY:
                    handleMobilityFromBelow(recMsg);
                    break;
                default:
                    LOG("Unknown Message type");
                    delete recMsg;
                    break;
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
                handleSenseFreeREPLY(recMsg);
                break;
            case SenseDataREPLY:
                handleSenseDataREPLY(recMsg);
                break;
            case SenseProposedREPLY:
                hanldeSenseProposedREPLY(recMsg);
                break;
            case SenseDrmREPLY:
                handleSenseDrmREPLY(recMsg);
                break;
            case PUSTART:
                handlePU();
                delete recMsg;
                break;
            default:
                delete recMsg;
                LOG("Unknown reply from SS");
                break;
        }
    }
    else if(msg->arrivedOn("sclInterface$i")){
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        drmChannel = recMsg->getProposedChannel();
        delete recMsg;
    }
    else if(msg->arrivedOn("mobilityCtrlInterface$i"))
    {
        ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
        switch(recMsg->getKind())
        {
            default:
                delete recMsg;
                break;
        }
    }
    else if(msg->arrivedOn("mobilityInterface$i"))
    {
        MobilityPacket *recMsg = check_and_cast<MobilityPacket *>(msg);
        switch(recMsg->getKind())
        {
            case MOB_DATA:
            case MOB_ACK:
            case MOB_NACK:
            case MOB_EVENT:
            case MOB_PUBLISH:
            case MOB_UPDATE:
            case MOB_KEEPALIVE:
                handleMobilityFromAbove(recMsg);
            case MOB_UNKNOWN:
                break;
            default:
                ASSERT(false);
        }
    }
}

void MobilityMacLayer::handleRTS(dataMsg *msg)
{

    if (currentState == RTS_SENT) // have sent my own RTS
    {
        LOG("Message received at node "<< myAddress << ", but I had sent my own RTS.");
        delete msg;
    }
    else if(currentState == DATA_RECEIVING && msg->getSource() != ctsDestination) // already receiving data from anothter node
    {
        LOG("Already receiving data, deleting RTS");
        delete msg;
    }
    else if(currentState == DATA_SENDING || currentState == RTS_SENDING) // already transmitting
    {
        LOG("Already transmitting data. delete RTS");
        delete msg;
    }
    else // I am idle, sense the proposed channel
    {
        setCurrentState(RTS_RECEIVED); // I am not a transmitter
        proposedChannel = msg->getProposedChannel();
        ctsDestination = msg->getSource();
        updateGUI(currentState, proposedChannel);
        delete msg;
        senseRequest(SenseProposedCHANNEL);
    }
}

void MobilityMacLayer::handleDataFromAbove(dataMsg *msg)
{
    switch(currentState)
    {
        case DATA_SENDING:
        {
            txDataQueue->insert(msg);
            sendData(MACDATA);
            break;
        }
        case DATA_RECEIVING:
            delete msg;
            break;
        case DATA_SENT:
            delete msg;
            LOG("handleData data_sent");
            break;
        case DATA_TIMEOUT:
            delete msg;
            LOG("handleData data_timeout");
            break;
        default:
            delete msg;
            LOG("handleData bad data state:" << MacLayerState_Names[currentState]);
            break;
    }

}

void MobilityMacLayer::handleDataFromBelow(dataMsg *msg)
{
    switch(currentState)
    {
        case DATA_SENDING:
            delete msg;
            break;
        case DATA_RECEIVING:
            rxDataQueue->insert(msg);
            processData();
            break;
        case DATA_SENT:
            delete msg;
            LOG("handleData data_sent");
            break;
        case DATA_TIMEOUT:
            delete msg;
            LOG("handleData data_timeout");
            break;
        default:
            delete msg;
            LOG("handleData bad data state:" << MacLayerState_Names[currentState]);
            break;
    }

}

void MobilityMacLayer::handleMobilityFromAbove(MobilityPacket *msg)
{
    txMobilityQueue->insert(msg);
    sendData(MOBILITY);
}

void MobilityMacLayer::handleMobilityFromBelow(dataMsg *msg)
{
    rxDataQueue->insert(msg);
    processData();
}

void MobilityMacLayer::processData()
{
    updateGUI(currentState,currentDataChannel);
    if ( rxDataQueue->isEmpty() )
    {
        LOG("ERROR: this should not happen!");
        ASSERT(false);
    }

    emit(totalRxPacketsSignal,1);

    if ( currentDataChannel != 0 )
    {
        dataMsg *frame = check_and_cast<dataMsg *>(rxDataQueue->pop());
        ASSERT(frame);
        switch( frame->getKind() )
        {
            case MOBILITY:
            {
                if (frame->hasEncapsulatedPacket())
                {
                    MobilityPacket *mobPacket = check_and_cast<MobilityPacket *>(frame->decapsulate());
                    ASSERT(mobPacket);
                    emit(rxMobilitySignal,1);
                    send(mobPacket,"mobilityInterface$o");
                }
                break;
            }
            case MACACK:
            case MACDATA:
            {
                emit(rxDataSignal, 1);
                int seqNo = frame->getNumberOfPackets();

                //Acknowlegde the received MACDATA frame if it need to be ack'ed
                sendACK(frame->getSource(),seqNo);

                if ( seqNo > rxSeqNo && frame->hasEncapsulatedPacket())
                {
                    //correct seqNo
                    rxSeqNo = seqNo;

                    dataMsg *payload = check_and_cast<dataMsg *>(frame->decapsulate());
                    //Send payload to upper layers
                    send(payload, "dataUpper$o");
                }
                else
                {
                    LOG("MACDATA frame with wrong seqNo received.  Do not send to upper layers");
                    LOG("TxSeq: " << txSeqNo << " | RxSeq: " << rxSeqNo << " | SeqNo: " << seqNo);
                }
                break;
            }
            default:
                break;
        }

        delete frame;
    }
}

void MobilityMacLayer::handleCTS(dataMsg *msg)
{
    setCurrentState(CTS_RECEIVED);
    // Check if the proposed channel is still free
    if(currentDataChannel != 0 && msg->getProposedChannel() == currentDataChannel){
        clearRTStimer(); // clear an RTS trigger to bring node to idle state.
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        setCurrentDataChannel(recMsg->getProposedChannel());
        delete recMsg;
        sendRendezvousCONFIRMATION();
        sendAppCONFIRMATION(); //Equals to sendData()
        setCurrentState(DATA_SENDING);
        rtsAttempts = par("rtsAttempts");
    }
    else{ // proposed channel is no longer free. Sense a new channel for sending
        LOG("Proposed channel is not free at transmitter anymore");
        if (rtsTimer != NULL)
        {
            cancelAndDelete(rtsTimer); rtsTimer = NULL;
        }
        delete msg;
        rtsAttempts = par("rtsAttempts");
        setCurrentState(RTS_SENDING);
        senseRequest(SenseFreeCHANNEL);
    }
}

void MobilityMacLayer::handleAck(dataMsg *msg)
{
    if (currentState == DATA_SENDING)
    {
        int seqNo = msg->getNumberOfPackets();

        //if not get expected seqNo, retransmit
        if (seqNo != txSeqNo)
        {
            sendData(MACDATA);
            return;
        }

        if (! txDataQueue->isEmpty() )
        {
            //Delete packet copy
            dataMsg *oldPacket = (dataMsg *) txDataQueue->pop();
            delete oldPacket;
        }

        // clear ACK timeout timer
        if(ackTimer != NULL)
        {
            cancelAndDelete(ackTimer);
            ackTimer = NULL;
        }

        ++txSeqNo %= 65536;

        sendAppCONFIRMATION(); //Equals to sendData()

        emit(rxAckSignal, 1);
        emit(totalRxPacketsSignal,1);

        //else{ // No more data to send. Notify App Layer of Success. And send EOT and Get Idle
            //sendEOT();
            //ev<<"MobilityMAC: All frames have been sent\n";
            //ctrlMsg *txFinish = new ctrlMsg("Tx Success");  // Inform Application layer about successful transmission of data.
            //txFinish->setKind(TxSUCCESS);
            //send(txFinish, "ctrlUpper$o");
            //getIdle();  // Come to an idle MAC state.
        //}
    }
    delete msg;
}

void MobilityMacLayer::handleNack(int type)
{
    emit(nackSignal, 1);
    emit(totalRxPacketsSignal,1);
    if (currentState == DATA_SENDING || currentState == RTS_SENDING || currentState == RTS_SENT)
    {
        switch(type)
        {
        case RTSNACK:
            LOG("NACK received, sending RTS on another channel");
            setCurrentState(RTS_SENDING);
            updateGUI(currentState,currentDataChannel);
            rtsAttempts = par("rtsAttempts");
            clearRTStimer();
            senseRequest(SenseFreeCHANNEL);
            break;
        case DATANACK: // implies a change of channel and not transmission related loss of data
            LOG("NACK received, sending Frame on another channel");
            emit(handover, 1);
            setCurrentState(HANDOVER);
            setCurrentDataChannel(0);
            updateGUI(currentState,currentDataChannel);
            clearAckTimer();
            sendRendezvousFAIL();
            senseRequest(SenseFreeCHANNEL);
            break;
        default:
            break;
        }
    }
    else if (currentState == DATA_RECEIVING)
    {
        getIdle();
    }
    else
        LOG("Problem in handleNack()");
}
void MobilityMacLayer::handleEOT()
{
    getIdle();
    this->getParentModule()->setDisplayString("i=abstract/penguin");
    dataMsg *eot = new dataMsg("EOT");
    eot->setKind(EOT); // No destination specified so that others can ignore it at mac layer
    send(eot, "dataLower$o");
}
void MobilityMacLayer::handlePU()
{
    if ( currentState == DATA_SENDING ) // I was transmitting
    {
        emit(handover, 1);
        if(ackEnabled == true){
            // cancel the ack timer, start RTS/CTS for a new channel.
            if (ackTimer != NULL)
            {
                cancelAndDelete(ackTimer);
                ackTimer = NULL;
            }
        }
        setCurrentState(HANDOVER);
        setCurrentDataChannel(0);
        updateGUI(currentState,currentDataChannel);
        rtsAttempts = par("rtsAttempts");
        sendRendezvousFAIL();

        if(drmEnabled == true)
            senseRequest(SenseDrmCHANNEL);
        else
            senseRequest(SenseFreeCHANNEL);
    }
    else if ( currentState == DATA_RECEIVING ) // I was receiving. Inform Tx if it is not affected by this PU
    {
        emit(handover, 1);
        sendRendezvousFAIL();
        sendNack(DATANACK);
        getIdle();
    }
    else { // Was neither tx nor rx. May be I only send RTS at this point.
        LOG("PU during DATA/RTS/CTS. Getting into idle state");
        getIdle();
    }
}

void MobilityMacLayer::handleAppREQUEST(ctrlMsg *msg)
{
    if ( currentState == DATA_RECEIVING) // postpone own communication
    {
        LOG("Postpone my own communication due to reception.");
    }
    else  // Attempt to start communication
    {
        setCurrentState(RTS_SENDING);

        currentDestination = msg->getDestination();
        rtsAttempts = par("rtsAttempts"); //Initialize rtsAttempts

        if(drmEnabled == true)
            senseRequest(SenseDrmCHANNEL);
        else  // DRM is not enabled. Sense any free channel
            senseRequest(SenseFreeCHANNEL);
    }
}

void MobilityMacLayer::handleSenseFreeREPLY(ctrlMsg *msg)
{
    if (msg->getChannelID() == 0)
    {
        LOG("No free channel available");
        getIdle();
    }
    else // Free channel found. If This node is not receiving, send RTS.
    {
        if (currentState == RTS_SENDING || currentState == HANDOVER)
        {
            setCurrentDataChannel(msg->getChannelID());
            sendRTS( msg->getChannelID() );
        }
        else
        {
            LOG("Receiving data from another. Postponing own communication");
            getIdle();
            notifyAppLayer(0);
        }
    }
    delete msg;
}

void MobilityMacLayer::hanldeSenseProposedREPLY(ctrlMsg * msg)
{
    if(msg->getChannelState() == true) // proposed channel is free.. Send CTS
    {
        delete msg;
        setCurrentDataChannel(proposedChannel);
        sendCTS();
    }
    else
    {
        if ( msg->getChannelState() != currentDataChannel )
        {
            delete msg;
            sendNack(RTSNACK);
        }
        getIdle();
    }
}

void MobilityMacLayer::handleSenseDataREPLY(ctrlMsg * msg)
{
    if(msg->getChannelState() == true) // Data channel is still free.
    {
        // Send Data/frame
        delete msg;
        sendData(0);
    }
    else // Data channel lost. What to do next?
    {
        LOG("Data channel lost. Handover needed");
        delete msg;
    }
}


void MobilityMacLayer::handleSenseDrmREPLY(ctrlMsg * msg)
{
    if (msg->getChannelState() == true)
    {
        if (isReceiving == false)
        {
            setCurrentDataChannel(drmChannel);
            sendRTS(drmChannel);
        }
        else
            LOG("Receiving data from another. Postponing own communication");
    }
    else{ // DRM channel is busy. Attempt again
        senseRequest(SenseFreeCHANNEL);
        //senseRequest(SenseDrmCHANNEL);
    }
    delete msg;
}

void MobilityMacLayer::setCurrentState(MacLayerState state)
{
    currentState = state;
    updateGUI(currentState, currentDataChannel);
}

void MobilityMacLayer::setCurrentDataChannel(int chID)
{
    currentDataChannel = chID;
    updateGUI(currentState, currentDataChannel);
}

void MobilityMacLayer::updateGUI(MacLayerState state, int chID)
{
    // GUI changes
    std::stringstream sstr;
    sstr << "i=abstract/penguin," << colorMap[chID] << ",40;t=Ch: " << chID
    << "\n" << MacLayerState_Names[state];
    std::string str1 = sstr.str();
    this->getParentModule()->setDisplayString(str1.c_str());
}

////////////////////////////////////////////////// SEND FUNCTIONS ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void MobilityMacLayer::sendRTS(int proposedChannel) // send RTS on a free channel, from spectrum sensing.
{
    if (rtsAttempts >= 1)
    {
        emit(rtsSignal, 1);
        dataMsg *rts = new dataMsg("RTS");
        rts->setSource(myAddress);
        rts->setDestination(currentDestination);
        rts->setProposedChannel(proposedChannel);
        rts->setKind(RTS);
        rts->setByteLength(65000);

        LOG("RTS to N-"<<rts->getDestination()<<" for channel "<<rts->getProposedChannel()<<". RTS attempt # "<< rtsAttempts);
        send(rts, "dataLower$o");
        rtsAttempts--;
        setCurrentState(RTS_SENT);
        setRTStimer();  // If no response is received, send another RTS if multiple RTS are enabled.

    }
    else // All rts attempts failed
    {
        getIdle();
        notifyAppLayer(0);
    }
}
void MobilityMacLayer::sendRendezvousCONFIRMATION()
{
    //Mobility Component notifiy
    LOG("Sending rendezvous confirmation to mobility component");
    ctrlMsg *msgMob = new ctrlMsg("RendezvousConf");
    msgMob->setKind(RendezvousSuccess);
    send(msgMob, "mobilityCtrlInterface$o");
}

void MobilityMacLayer::sendRendezvousFAIL()
{
    //Mobility Component notifiy
    LOG("Sending rendezvous fail/broken to mobility component");
    ctrlMsg *msgMob = new ctrlMsg("RendezvousFail");
    msgMob->setKind(RendezvousFail);
    send(msgMob, "mobilityCtrlInterface$o");
}

void MobilityMacLayer::sendAppCONFIRMATION()
{
    //App notify
    LOG("Sending rendezvous confirmation for Application start");
    ctrlMsg *msgApp = new ctrlMsg("AppConf");
    msgApp->setKind(AppCONFIRMATION);
    send(msgApp, "ctrlUpper$o");
}

void MobilityMacLayer::sendCTS()
{
    setCurrentState(CTS_SENT);
    dataMsg *cts = new dataMsg("CTS");
    cts->setKind(CTS);
    cts->setSource(myAddress);
    cts->setDestination(ctsDestination);
    cts->setProposedChannel(currentDataChannel);
    cts->setByteLength(34);
    LOG("Sending CTS to "<<cts->getDestination());
    send(cts, "dataLower$o");
    setCurrentState(DATA_RECEIVING);
    rtsAttempts = par("rtsAttempts");
    sendRendezvousCONFIRMATION();
}

void MobilityMacLayer::sendData(int kind = 0)
{
    updateGUI(currentState,currentDataChannel);

    // if channel is lost or there if nothing to do
    // Do not send a packet
    if (currentDataChannel == 0)
    {
        LOG("Abort data transmission. Data channel is lost: currentDataChannel:" << currentDataChannel);
        txDataQueue->clear();
        getIdle();
        return;
    }

    // if sense per packet is disabled, send next packet.. otherwise sense the data channel
    if (perPacketSensing == true)
    {
        // send data channel sense request
        senseRequest(SenseDataCHANNEL);
        return;
    }

    dataMsg *frame = new dataMsg();

    bool txFrame,needACK = false;
    switch( kind )
    {
        case MOBILITY:
            if ( ! txMobilityQueue->isEmpty() )
            {
                txFrame = true;
                frame->setName("MAC-MOBILITY-FRAME");
                MobilityPacket * payload = (MobilityPacket *) txMobilityQueue->pop();
                ASSERT(payload);
                frame->setDestination(payload->getDestination());
                frame->encapsulate(payload);
                emit(txMobilitySignal,1);
            }
            else
            {
                LOG("Nothing to send");
                getIdle();
            }
            break;
        case MACDATA:
            if ( ! txDataQueue->isEmpty() )
            {
                if (currentState == DATA_SENDING)
                {
                    needACK = true;
                    txFrame = true;
                    frame->setName("MAC-DATA-FRAME");
                    dataMsg * payload = (dataMsg *) txDataQueue->front()->dup();
                    ASSERT(payload);
                    frame->setDestination(payload->getDestination());
                    frame->encapsulate(payload);
                    emit(txDataSignal, 1); //Send DATA packetSignal
                }
            }
            else
            {
                LOG("Nothing to send");
                getIdle();
            }
            break;
        default:
            break;
    }

    if (txFrame)
    {
        frame->setKind(kind);
        frame->setSource(myAddress);
        frame->setProposedChannel(currentDataChannel);

        //encapsulate the packet and forward it to the next node

        if (needACK && ackEnabled)
        {
            frame->setByteLength(2000);
            frame->setNumberOfPackets( txSeqNo );
            setAckTimeOut();
        }
        else
        {
            //Don't need ACK, therefore delete the reference in the txQueue
            if (kind != MOBILITY)
            {
                dataMsg *oldPacket = (dataMsg *) txDataQueue->pop();
                delete oldPacket;
            }
        }
        send(frame, "dataLower$o");
        emit(totalTxPacketsSignal,1);
    }
    else
        delete frame;
}

void MobilityMacLayer::sendNack(int type)
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
     emit(totalTxPacketsSignal,1);
     getIdle();
}

void MobilityMacLayer::sendEOT(){
    // send End of Transmission Message
    dataMsg *eot = new dataMsg("EOT");
    eot->setSource(myAddress); eot->setDestination(currentDestination);
    eot->setProposedChannel(currentDataChannel);
    eot->setKind(EOT);
    send(eot, "dataLower$o");
}

////////////////////////////////////////////// UTILITY FUNCTIONS ////////////////////////////////////////
void MobilityMacLayer::senseRequest(int type)
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
            LOG("Unknown sensing request");
            break;
    }
    send(senseRequest, "ssInterface$o");
}
void MobilityMacLayer::getIdle()
{
    this->getParentModule()->setDisplayString("i=abstract/penguin");
    if (rtsTimer != NULL)
    {
        cancelAndDelete(rtsTimer); rtsTimer = NULL;
    }
    if (ackTimer != NULL){
        cancelAndDelete(ackTimer); ackTimer = NULL;
    }
    rtsAttempts = par("rtsAttempts");
    setCurrentDataChannel(0);
    setCurrentState(IDLE);
    updateGUI(currentState,currentDataChannel);
    rtsTimer = ackTimer = NULL;
}

/////////////////////////////////////////////////// TIMERS  //////////////////////////////////////////
void MobilityMacLayer::setRTStimer()  // is called when RTS is sent.
{
    if (rtsTimer == NULL)
    {
        LOG("RTS Timeout timer set");
        rtsTimer = new timerMsg("RTS-timer");
        scheduleAt(simTime()+ 0.3, rtsTimer);
    }
}
void MobilityMacLayer::clearRTStimer()
{
    if(rtsTimer != NULL)
    {
        LOG("RTS timer cleared");
        cancelAndDelete(rtsTimer);
        rtsTimer = NULL;
    }
}
void MobilityMacLayer::clearAckTimer()
{
    if(ackTimer != NULL)
    {
        LOG("AcK timer cleared");
        cancelAndDelete(ackTimer);
        ackTimer = NULL;
    }
}
void MobilityMacLayer::setAckTimeOut()
{
    if (ackTimer == NULL)
    {
        ackTimer = new timerMsg("Ack-timeout");
        scheduleAt(simTime()+0.12, ackTimer);
    }
}
//////////////////////////////////////////////////////////////////
void MobilityMacLayer::notifyAppLayer(int type){
    ctrlMsg *txStatus = new ctrlMsg("TxFail");
    switch(type){
    case 0:  //Tx fail
        LOG("notifyAppLayer Tx FAIL");
        txStatus->setKind(TxFAIL);
        break;
    case 1:  // Tx Success
        LOG("notifyAppLayer Tx SUCCESS");
        txStatus->setKind(TxSUCCESS);
        break;
    default:
        break;
    }
    send(txStatus, "ctrlUpper$o");
}
void MobilityMacLayer::finish(){
    getIdle();
}

void MobilityMacLayer::sendACK(int source, int seqNo)
{
    if(ackEnabled == true)
    {
        dataMsg *frame = new dataMsg();
        frame->setName("MACACK");
        frame->setBitLength(300);
        frame->setKind(MACACK);
        frame->setSource(myAddress);
        frame->setDestination( source );
        frame->setNumberOfPackets( seqNo );
        frame->setProposedChannel(currentDataChannel);
        //Reply to sender with an ACK
        send(frame, "dataLower$o");
        emit(txAckSignal, 1);
    }
}









