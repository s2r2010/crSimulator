
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * This module is responsible to keep track of the channel state and provide this information to any module requesting it. This implementation works with "drmMacLayer" and with
 * the DRM module. It provides regular spectrum sensing updates to the DRM module so that DRM can aggregate them over time and characterize the channels accordingly
 */

#include "drmSpecSensor.h"

void drmSpecSensor::initialize()
{
    init();
    //publishSensingResults();
}
void drmSpecSensor::handleMessage(cMessage *msg)
{
    if (msg == freeSenseTimer){
        delete msg; freeSenseTimer = NULL; senseChannel(SenseFreeCHANNEL);
    }
    else if(msg == dataSenseTimer){
        delete msg; dataSenseTimer = NULL; senseChannel(SenseDataCHANNEL);
    }
    else if(msg == proposedSenseTimer){
        delete msg; proposedSenseTimer = NULL; senseChannel(SenseProposedCHANNEL);
    }
    else if (msg == drmSenseTimer){
        delete msg; drmSenseTimer = NULL; senseChannel(SenseDrmCHANNEL);
    }
    //else if(msg == publishTimer){
        //delete msg; publishTimer = NULL; publishSensingResults();
    //}
    // Ignore incoming control messages for channel availability decisions
    // Data messages result in channel busy/idle states
    else if(msg->arrivedOn("phyInterface$i")) // a msg from outside world through
    {
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        if(recMsg->getDestination() == myAddress)  // Do not change status of channels.
        {
            if(recMsg->getKind() == DATA)
            {
                currentDataChannel = recMsg->getProposedChannel();
            }
            else if(recMsg->getKind() == CTS)
            {
                currentDataChannel = recMsg->getProposedChannel();
            }
            delete recMsg;
        }
        else  // Communication by other nodes. Causes channel availability adjustments.
        {
            // Msg for other nodes.
            switch(recMsg->getKind())
            {
                case RTS:
                    delete recMsg;
                    break;
                case CTS: // Channel state busy
                    channelState(recMsg->getProposedChannel(), 1);
                    ev<<"SS: Channel "<<recMsg->getProposedChannel()-1<<" state is now: 1\n";
                    delete recMsg;
                    break;
                case ACK:
                    ev<<"SS: ACK for other nodes. Setting proposed channel "<<recMsg->getProposedChannel()<<" as busy.\n";
                    channelState(recMsg->getProposedChannel(), 1);
                    delete recMsg;
                    break;
                case RTSNACK:
                    delete recMsg;
                    break;
                case DATANACK:
                    delete recMsg;
                    break;
                case DATA:
                    ev<<"SS: DATA for other nodes. Setting proposed channel "<<recMsg->getProposedChannel()<<" as busy.\n";
                    channelState(recMsg->getProposedChannel(), 1);
                    delete recMsg;
                    break;
                case EOT:
                    ev<<"SS: EOT for other nodes. Setting proposed channel "<<recMsg->getProposedChannel()<<" as free.\n";
                    channelState(recMsg->getProposedChannel(), 0);
                    delete recMsg;
                    break;
                case PUSTART:
                    ev<<"SS: PU Communication. Setting PU channel "<<recMsg->getProposedChannel()<<" as busy.\n";
                    channelState(recMsg->getProposedChannel(), 1);
                    // Inform the mac Layer
                    if (recMsg->getProposedChannel() == currentDataChannel)
                    {
                        currentDataChannel = 0;
                        ctrlMsg *puAppear = new ctrlMsg("PU Arrived");
                        puAppear->setKind(PUSTART);
                        send(puAppear, "macInterface$o");
                    }
                    delete recMsg;
                    break;
                case PUEND:
                    ev<<"SS: PU Communication ends. Setting PU channel "<<recMsg->getProposedChannel()<<" as free.\n";
                    channelState(recMsg->getProposedChannel(), 0);
                    delete recMsg;
                    break;
                default:
                    ev<<"SS: Unknown message from outside world.\n";
                    delete recMsg;
                    break;
            }
        }
    }
    else if (msg->arrivedOn("macInterface$i"))  // A control msg from MAC Layer.
    {
        ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
        switch(recMsg->getKind()){
        case SenseFreeCHANNEL:
            sensedChannel = intuniform(0, totalChannels-1); // So that sensing always begins from a random channel and not from channel 1.
            scheduleSensing(SenseFreeCHANNEL);
            break;
        case SenseDataCHANNEL:
            currentDataChannel = recMsg->getChannelID();
            scheduleSensing(SenseDataCHANNEL);
            break;
        case SenseProposedCHANNEL:
            proposedChannel = recMsg->getChannelID();
            ev<<"Proposed channel is "<<recMsg->getChannelID()<<"\n";
            scheduleSensing(SenseProposedCHANNEL);
            break;
        case SenseDrmCHANNEL:
            drmChannel = recMsg->getChannelID();
            scheduleSensing(SenseDrmCHANNEL);
            break;
        default:
            ev<<"SS: Unknown ctrl msg from MAC\n";
            break;
        }
        delete recMsg;
    }
    else
    {
        ev<<"PROBLEM in SS\n";
        delete msg;
    }

    // Types of sensing request. Sense particular channel, Sense until finding free channel; sensing based on DRM recommendation
    // Based upon the request of MAC or configuration
}
void drmSpecSensor::channelState(int chID, int chState)
{
    channelsArray[chID-1] = chState;
}
void drmSpecSensor::senseChannel(int type)   // NEED TO TAKE CARE OF WHEN TO CANCEL THESE TIMERS IN CASE THE NODE BECOMES RECEIVER.
{
    ctrlMsg *senseReply = new ctrlMsg("Sense-Reply");
    switch(type)
    {
    case SenseFreeCHANNEL:   // sense any free channel
        if(channelsArray[sensedChannel] != 0)  // The currently sensed channels is either busy of occupied by PU
        {
            sensingIteration++;
            if (sensingIteration > 3*totalChannels)  // Convert the 3* into a parameter
            {
                // stop and notify app layer
                sensingIteration = 0; // reset
                ev<<"All Channels are busy and not turning free.\n";
                senseReply->setChannelState(false);
                senseReply->setKind(SenseFreeREPLY);
                senseReply->setChannelID(0);
                send(senseReply, "macInterface$o");
            }
            else  // we can continue to look for next channel to sense
            {
                emit(sensingSignal, sensedChannel+1);
                delete senseReply;
                ev<<"Channel "<<sensedChannel+1<<" is busy. Sensing another\n";
                sensedChannel++;
                if (sensedChannel >= totalChannels)
                    sensedChannel = 0;
                scheduleSensing(SenseFreeCHANNEL);  // set another timer to sense next channel.
            }
        }
        else  // the sensed channel is free. Notify MAC Layer and give the channel ID
        {
            emit(sensingSignal, sensedChannel+1);
            ev<<"Channel "<<sensedChannel+1<<" is free.\n"; currentDataChannel = sensedChannel+1;
            senseReply->setChannelID(sensedChannel+1);
            senseReply->setKind(SenseFreeREPLY);
            send(senseReply, "macInterface$o");
        }
        break;
    case SenseDataCHANNEL:   // sense the state of SU channel
        if(channelsArray[currentDataChannel-1] == 0)  // data channel is still free.
        {
            emit(sensingSignal, currentDataChannel);
            ev<<"Channel "<<currentDataChannel<<" is still free for communication\n";
            senseReply->setChannelState(true);
            senseReply->setKind(SenseDataREPLY);
            send(senseReply, "macInterface$o");
        }
        else // Lost data channel to PU.
        {
            sensingIteration = 0;
            emit(sensingSignal, currentDataChannel);
            ev<<"Channel "<<currentDataChannel<<" is no longer free for communication\n";
            senseReply->setChannelState(false);
            senseReply->setKind(SenseDataREPLY);
            send(senseReply, "macInterface$o");
        }
        break;
    case SenseProposedCHANNEL:
        if(channelsArray[proposedChannel-1] == 0)
        {
            // Proposed channel is free
            currentDataChannel = proposedChannel;
            emit(sensingSignal, proposedChannel);
            ev<<"Proposed Channel "<<proposedChannel<<" is free\n";
            senseReply->setChannelState(true);
            senseReply->setKind(SenseProposedREPLY);
            send(senseReply, "macInterface$o");
        }
        else
        {
            ev<<"Proposed channel "<<proposedChannel<<" is busy\n";
            emit(sensingSignal, proposedChannel);
            senseReply->setChannelState(false);
            senseReply->setKind(SenseProposedREPLY);
            send(senseReply, "macInterface$o");
        }
        break;
    case SenseDrmCHANNEL:
        if(channelsArray[drmChannel-1] == 0)            // DRM channel is free
        {
            currentDataChannel = drmChannel;
            emit(sensingSignal, drmChannel);
            ev<<"DRM Channel "<<drmChannel<<" is free\n";
            senseReply->setChannelState(true);
        }
        else
        {
            ev<<"DRM channel "<<drmChannel<<" is busy\n";
            emit(sensingSignal, drmChannel);
            senseReply->setChannelState(false);
        }
        senseReply->setKind(SenseDrmREPLY);
        send(senseReply, "macInterface$o");
        break;
    default:
        ev<<"Unknown channel sense request\n";
        break;
    }
}
void drmSpecSensor::scheduleSensing(int type)
{
    switch(type){
    case SenseFreeCHANNEL:   // sense any free channel. sense timer 1
        sensingIteration = 0;
        freeSenseTimer = new timerMsg("Sensing-Free");
        freeSenseTimer->setKind(SenseFreeCHANNEL);
        scheduleAt(simTime()+sensingDuration, freeSenseTimer);
        break;
    case SenseDataCHANNEL:   // sense the operating channel. sense timer 2
        dataSenseTimer = new timerMsg("Sensing-Data");
        dataSenseTimer->setKind(SenseDataCHANNEL);
        scheduleAt(simTime()+sensingDuration, dataSenseTimer);
        break;
    case SenseProposedCHANNEL:
        proposedSenseTimer = new timerMsg("Sense-Proposed");
        proposedSenseTimer->setKind(SenseProposedCHANNEL);
        scheduleAt(simTime()+sensingDuration, proposedSenseTimer);
        break;
    case SenseDrmCHANNEL:
        drmSenseTimer = new timerMsg("Sense-DRM");
        drmSenseTimer->setKind(SenseDrmCHANNEL);
        scheduleAt(simTime()+sensingDuration, drmSenseTimer);
        break;
    default:  // nothing
        break;
    }
}
void drmSpecSensor::init()
{
    totalChannels = getParentModule()->getParentModule()->par("totalChannels");
    drmChannel = 1;
    channelsArray = new int[totalChannels];
    sensingDuration = par("sensingDuration");
    myAddress = getParentModule()->par("address");
    freeSenseTimer = dataSenseTimer = proposedSenseTimer = NULL;//publishTimer = NULL;  // timers for sensing free or data channels.
    sensedChannel = sensingIteration = 0;
    proposedChannel = 0;
    currentDataChannel = 0;
    sensingSignal = registerSignal("sensingSignal");
    // Set all channels to be free at the beginning
    for (int x=0; x<totalChannels; x++){
        channelsArray[x] = 0;
    }
}
void drmSpecSensor::publishSensingResults()
{
    // Publish the channel state array on the SCL.
//    ctrlMsg *sensingResult = new ctrlMsg("toDRM");
//    sensingResult->setKind(SensingRESULT);
//    sensingResult->setSensingResultArraySize(totalChannels);
//    for(int x = 0; x<totalChannels; x++){
//        sensingResult->setSensingResult(x, channelsArray[x]);
//    }
//    send(sensingResult, "sclInterface$o");

    // schedule a timer for next publish event
//    publishTimer = new timerMsg("sensingResults");
//    scheduleAt(simTime()+(totalChannels*sensingDuration), publishTimer);
}


