
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * An RTS/CTS based Mac implementation that is connected with Spectrum Sensing Module and can avoid Primary Users by switching channels. Channel switching/mobility
 * is random.
 */

#include "crMacLayer.h"

void crMacLayer::initialize()
{
    drmEnabled = par("drmEnabled");
    ackEnabled = par("AckEnabled");
    totalFrames = par("totalFrames"); // Number of packets per transmission session.
    rtsAttempts = par("rtsAttempts");
    perPacketSensing = par("sensePerPacket");
    myAddress = getParentModule()->par("address");

    numberOfPackets = currentDestination = frameSendAttempts = currentDataChannel = proposedChannel = ctsDestination = 0;     // No frames to send at the time of initialization
    isReceiving = isTransmitting = rtsSent = false;  // set to true when receiving/receiving data from another or when RTS is sent.

    rtsTimer = ackTimer = NULL;
    frame = NULL;

    // Statistics
    rtsSignal = registerSignal("rtsSignal");
    handover = registerSignal("handover");
    nackSignal = registerSignal("nackSignal");
    dataSignal = registerSignal("dataSignal");
}
/////////////////////////////////////////////////// HANDLERS   /////////////////////////////////////////

void crMacLayer::handleMessage(cMessage *msg)
{
    if (msg == rtsTimer)  // No response received from destination node for the sent RTS.
    {
        delete msg;
        ev<< "Node: "<<myAddress<<". No response to my sent RTS\n";
        rtsTimer = NULL;
        // ATTEMPT AGAIN
        if (rtsAttempts >= 1)
            senseRequest(SenseFreeCHANNEL);
    }
    if (msg == ackTimer)  // Ack timer fired and no response received... Send frame again. Ack timer should be scheduled only when ACks are enabled.
    {
        delete msg;
        ackTimer = NULL;
        sendData();
    }
    else if(msg->arrivedOn("ctrlUpper$i")) // ctrl msg from network layer
    {
            ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
            if (recMsg->getKind() == AppREQUEST) // A request for sending some data
            {
                if (isReceiving == true)
                {
                    // postpone own communication
                    ev<<"Postpone my own communication due to reception.\n";
                    delete recMsg;
                }
                else
                {
                    // I have to send some data to the destination given in the request. Store the destination for the duration of transmission.
                    currentDestination = recMsg->getDestination();

                    // How much data: Fixed or Variable!  Answer comes from configuration.
                    numberOfPackets = par("totalFrames");

                    // How many RTS attempts? Answer comes from configuration file
                    // Frames are acknowledged or not?
                    // Store the current destination and send an RTS for the amount of data.
                    frameSendAttempts = 0;
                    delete recMsg;
                    // send a sensing request to Physical Layer
                    // WHICH CHANNEL TO SENSE ?
                    // ANY FREE CHANNEL AVAILABLE OR DRM based RECOMMENDATION (drmEnabled = True or False)
                    if(drmEnabled == true)
                    {
                        // Get the channel recommendation from DRM array
                        // Sense recommended channel
                    }
                    else  // DRM is not enabled. Sense any free channel
                    {
                        // Sense any free channel from PhyLayer array
                        senseRequest(SenseFreeCHANNEL);
                    }
                }
            }
    }
    else if(msg->arrivedOn("dataUpper$i"))
    {
        // now we need to see if data channel is still free.. if yes.. send the frame and wait for ack..
        ctrlMsg *senseData = new ctrlMsg("Sense-Data-Channel");
        senseData->setKind(101); senseData->setChannelID(currentDataChannel);
        send(senseData, "ctrlLower$o");
    }
    else if(msg->arrivedOn("ctrlLower$i")) // ctrl msg from Physical Layer
    {
        ev<<"MAC: Unknown ctrl msg from PHY\n";
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
             case ACK:  if(recMsg->getNumberOfPackets() == 1) // ACK for last packet.
             {
                 isTransmitting = false;
                 currentDestination = false;
             }
             delete recMsg;
             handleAck();
             break;
             case RTSNACK:  handleNack(); delete recMsg; break;
             case EOT: handleEOT(); delete recMsg; break;
             case PUSTART: handlePU(); delete recMsg; break;
             case PUEND: delete recMsg; break;
             default: ev<<"Unknown Message type\n"; delete recMsg; break;
             }
        }
        else        // Message for others.
        {
            delete recMsg;
        }
    }
    else if(msg->arrivedOn("ssInterface$i"))
    {
        ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
        switch(recMsg->getKind())
        {
            case SenseFreeREPLY:
                if (recMsg->getChannelID() == 0)
                {
                    ev<<"No free channel available\n";
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
                    sendNack();
                }
                break;
            case PUSTART:
                handlePU();
                delete recMsg;
                break;
            default:
                delete recMsg;
                break;
        }
    }
}

void crMacLayer::handleRTS(dataMsg *msg)
{
    ev<<"Handling RTS HERE\n";
    if (rtsSent == true) // have sent my own RTS
    {   ev<< "Message received at node "<< myAddress << ", but I had sent my own RTS.\n";
        delete msg;
    }
    else if(isReceiving == true) // already receiving data from anothter node
    {   ev<<"Node "<<myAddress<<": Already receiving data, deleting RTS\n";
        delete msg;
    }
    else if(isTransmitting == true) // already transmitting
    {   ev<<"Node "<<myAddress<<": I am already transmitting data. delete RTS\n";
        delete msg;
    }
    else // I am idle, sense the proposed channel
    {
        rtsSent = false; // I am not a transmitter
        isReceiving = true;
        proposedChannel = msg->getProposedChannel();
        ctsDestination = msg->getSource();
        delete msg;
        senseRequest(SenseProposedCHANNEL);
    }
}
void crMacLayer::handleData(dataMsg *msg)
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
    }
    else
    {
        delete recMsg;
    }
}
void crMacLayer::handleCTS(dataMsg *msg)
{
    clearRTStimer(); // clear an RTS trigger to bring node to idle state.
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        rtsSent = false;
        currentDataChannel = recMsg->getProposedChannel();
        delete recMsg;
        // If perPacket Sending is enabled Then sense the channel before calling sendDATA
        //if(perPacketSensing == true)
        //{
            // sense the data channel
        //}
        //else
        //{
            // sendData();
        //}
        sendData();
}
void crMacLayer::handleAck()
{
    // clear ACK timeout timer
    if(ackTimer != NULL)
    {
        cancelAndDelete(ackTimer);
        ackTimer = NULL;
        ev<<"Ack timeout cleared\n";
    }
    numberOfPackets--;
    frameSendAttempts = par("rtsAttempts"); // Reset the frame send attempts for next packet.

    if (currentDataChannel == 0) // channel is lost. Dont send next packet
    {    }
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
void crMacLayer::handleNack()
{
    emit(nackSignal, 1);
    ev<<"Node "<<myAddress<<": NACK received, sending RTS on another channel\n";
    rtsSent=false;
    rtsAttempts = par("rtsAttempts");
    clearRTStimer();
    ctrlMsg *senseChannel = new ctrlMsg("SenseChannel");  senseChannel->setKind(SenseFreeCHANNEL);
    send(senseChannel, "ssInterface$o");
}
void crMacLayer::handleEOT()
{
    dataMsg *eot = new dataMsg("EOT");
    eot->setKind(EOT); // No destination specified so that others can ignore it at mac layer
    send(eot, "dataLower$o");
}
void crMacLayer::handlePU()
{
    if(isTransmitting == true){  // I was transmitting
        emit(handover, 1);
        if(ackEnabled == true){
            // cancel the ack timer, start RTS/CTS for a new channel.
            if (ackTimer != NULL){ cancelAndDelete(ackTimer); ackTimer = NULL;}
            currentDataChannel = 0;
            rtsAttempts = par("rtsAttempts");
            senseRequest(SenseFreeCHANNEL);
        }
        else{  // stop the ack less trans
            ev<<"MAC: Code missing for handlePU()\n";
        }
    }
    else {  // I was receiving
        emit(handover, 1);
        getIdle();
    }

}
////////////////////////////////////////////////// SEND FUNCTIONS ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void crMacLayer::sendRTS(int proposedChannel) // send RTS on a free channel, from spectrum sensing.
{
    ev<<"MAC: RTS Attempts left are "<<rtsAttempts<<"\n";
    isTransmitting = true;
    if (rtsAttempts >= 1)
    {
        emit(rtsSignal, 1);
        dataMsg *rts = new dataMsg("RTS");
        rts->setSource(myAddress);
        rts->setDestination(currentDestination);
        rts->setProposedChannel(proposedChannel);
        rts->setKind(RTS);  // zero for rts

        ev<<"Node "<<myAddress<<": RTS to N-"<<rts->getDestination()<<" on "<<rts->getProposedChannel()<<". RTS attempt # "<< rtsAttempts <<"\n";
        send(rts, "dataLower$o");
        rtsAttempts--;
        rtsSent = true;
        setRTStimer();  // If no response is received, send another RTS if multiple RTS are enabled.
    }
    else // All rts attempts failed. Inform app layer by sending a nack
    {
        //emit(rtsFailSignal, 1);
        ev<<"All RTS attempts failed. Notifying App Layer\n";
        rtsSent = false;
        isTransmitting = false;
        rtsAttempts = par("rtsAttempts"); // Re-initialize the number of RTS attempts for next session
        ctrlMsg *nack = new ctrlMsg("RTS-Failure");
        nack->setKind(4);  // 4 tells to postpone its communication because of congestion
        send(nack, "ctrlUpper$o");
    }
}
void crMacLayer::sendCTS()
{
   dataMsg *cts = new dataMsg("CTS");
   cts->setKind(CTS);
   cts->setSource(myAddress);
   cts->setDestination(ctsDestination);
   cts->setProposedChannel(currentDataChannel);
   ev<<"Node "<<myAddress<<": Sending CTS to "<<cts->getDestination()<<".\n";
   send(cts, "dataLower$o");
   isReceiving = true;
}
void crMacLayer::sendData()
{
    if (frame == NULL)  // First call to send data. Initialize forwarding of first packet
    {
        frame = new dataMsg("DATA");
        frame->setSource(myAddress);
        frame->setKind(DATA);
        frame->setDestination(currentDestination);
        numberOfPackets = totalFrames;
        frameSendAttempts = par("rtsAttempts");
    }
    if (numberOfPackets == 0)  // All data has been sent.
    {
        // send End of Transmission Message
        dataMsg *eot = new dataMsg("EOT");
        eot->setSource(myAddress); eot->setDestination(currentDestination);
        eot->setProposedChannel(currentDataChannel);
        eot->setKind(EOT);
        send(eot, "dataLower$o");
        ev<<"MAC: All frames have been sent\n";
        ctrlMsg *txFinish = new ctrlMsg("Tx Success");  // Inform Application layer about successful transmission of data.
        txFinish->setKind(TxSUCCESS);
        send(txFinish, "ctrlUpper$o");
        getIdle();  // Come to an idle MAC state.
    }
    else  // send the next message and set the ACK timer if needed.
    {
        if (ackEnabled == true) // Send the next packet only after receiving ACK
        {
            if(frameSendAttempts>0)  // Frame sending retries....
            {
                // Check if data channel is free....
                if (currentDataChannel != 0){ // Channel is free, send packet.
                    ev<<"Sending copy of frame and setting ACK timeout\n";
                    frame->setNumberOfPackets(numberOfPackets);
                    frame->setProposedChannel(currentDataChannel);
                    dataMsg *copy = (dataMsg *) frame->dup();
                    send(copy, "dataLower$o");
                    frameSendAttempts--;
                    setAckTimeOut();
                }
                else{
                    ev<<"MAC: Data channel is lost to PU\n";
                }
            }
            else
            {
                ev<<"MAC: No ack received for sent packets. Aborting\n";
                getIdle();
            }

        }
        else // No acknowledgements. Send the packet and send another after it.
        {
            for (int x = 0; x<totalFrames; x++)
            {
                dataMsg *data = (dataMsg *) frame->dup();
                data->setNumberOfPackets(numberOfPackets);
                data->setKind(DATA);
                numberOfPackets--;
                sendDelayed(data, simTime()+0.2, "dataLower$o");
            }
            getIdle();
        }

    }
}
void crMacLayer::sendNack()
{
    isReceiving = false;
     dataMsg *nack = new dataMsg("Nack");
     nack->setKind(RTSNACK);
     nack->setDestination(ctsDestination);
     send(nack, "dataLower$o");
}

////////////////////////////////////////////// UTILITY FUNCTIONS ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void crMacLayer::senseRequest(int type)
{
    ctrlMsg *senseRequest = new ctrlMsg("SenseRequest");
    switch (type)
    {
        case SenseFreeCHANNEL:  // sense any free channel request
            senseRequest->setKind(SenseFreeCHANNEL);
            send(senseRequest, "ssInterface$o");
            break;
        case SenseDataCHANNEL:  // sense the data channel request
            senseRequest->setKind(SenseDataCHANNEL);
            senseRequest->setChannelID(currentDataChannel);
            send(senseRequest, "ssInterface$o");
            break;
        case SenseProposedCHANNEL:  // sense the channel proposed by Tx
            senseRequest->setKind(SenseProposedCHANNEL);
            senseRequest->setChannelID(proposedChannel);
            send(senseRequest, "ssInterface$o");
            break;
        default:
            ev<< "SS: Unknown sensing request\n";
            break;
    }
}
void crMacLayer::getIdle()
{
    if (frame != NULL)
    {
        delete frame;  frame = NULL;
    }
    if (rtsTimer != NULL)
    {
        //if (rtsTimer->isScheduled() == true)
        //{
            //cancelAndDelete(rtsTimer); rtsTimer = NULL;
        //}
        //else
        //{
            cancelAndDelete(rtsTimer); rtsTimer = NULL;
        //}
    }
    if (ackTimer != NULL){
        delete ackTimer; ackTimer = NULL;
    }
    rtsAttempts = frameSendAttempts = currentDestination = currentDataChannel = 0;
    isReceiving = isTransmitting = rtsSent = false;
    rtsTimer = ackTimer = NULL;
}

/////////////////////////////////////////////////// TIMERS  //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void crMacLayer::setRTStimer()  // is called when RTS is sent.
{
    if (rtsTimer == NULL)
    {
        ev<<"RTS Timeout timer set\n";        rtsTimer = new timerMsg("RTS-timer");        scheduleAt(simTime()+ 0.3, rtsTimer);
    }
}
void crMacLayer::clearRTStimer()
{
    if(rtsTimer != NULL)
    {
        ev<<"RTS timer cleared\n";        cancelAndDelete(rtsTimer);        rtsTimer = NULL;
    }
}
void crMacLayer::setAckTimeOut()
{
    if (ackTimer == NULL)
    {
        ackTimer = new timerMsg("Ack-timeout");        scheduleAt(simTime()+0.12, ackTimer);
    }
}
 //////////////////////////////////////////////////////////////////







