
/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * This module is responsible to keep track of the channel state and provide this information to any module requesting it. This implementation works with "drmMacLayer" and with
 * the DRM module. It provides regular spectrum sensing updates to the DRM module so that DRM can aggregate them over time and characterize the channels accordingly
 */

#include "MobilitySpecSensor.h"

void MobilitySpecSensor::initialize()
{
    init();
    //publishSensingResults();
}
void MobilitySpecSensor::handleMessage(cMessage *msg)
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
    else if (msg == mobilitySenseTimer){
        delete msg; mobilitySenseTimer = NULL; senseChannel(SenseChannelMap);
    }
    //else if(msg == publishTimer){
        //delete msg; publishTimer = NULL; publishSensingResults();
    //}
    // Ignore incoming control messages for channel availability decisions
    // Data messages result in channel busy/idle states
    else if(msg->arrivedOn("phyInterface$i")) // a msg from outside world through
    {
        dataMsg *recMsg = check_and_cast<dataMsg *>(msg);
        if(recMsg->getDestination() == myAddress || recMsg->getDestination() == broadcastAddress)  // Do not change status of channels.
        {
            switch(recMsg->getKind())
            {
                case DATA:
                case CTS:
                case MACDATA:
                    currentDataChannel = recMsg->getProposedChannel();
                    break;
                default:
                    break;
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
                    LOG("SS: Channel "<<recMsg->getProposedChannel()-1<<" state is now: 1");
                    delete recMsg;
                    break;
                case MACACK:
                case ACK:
                    LOG("SS: ACK for other nodes. Setting proposed channel "<<recMsg->getProposedChannel()<<" as busy.");
                    channelState(recMsg->getProposedChannel(), 1);
                    delete recMsg;
                    break;
                case RTSNACK:
                    delete recMsg;
                    break;
                case DATANACK:
                    delete recMsg;
                    break;
                case MACDATA:
                case DATA:
                    LOG("SS: DATA for other nodes. Setting proposed channel "<<recMsg->getProposedChannel()<<" as busy.");
                    channelState(recMsg->getProposedChannel(), 1);
                    delete recMsg;
                    break;
                case EOT:
                    LOG("SS: EOT for other nodes. Setting proposed channel "<<recMsg->getProposedChannel()<<" as free.");
                    channelState(recMsg->getProposedChannel(), 0);
                    delete recMsg;
                    break;
                case PUSTART:
                    LOG("SS: PU Communication. Setting PU channel "<<recMsg->getProposedChannel()<<" as busy.");
                    channelState(recMsg->getProposedChannel(), 1);
                    // Inform the mac Layer
                    notifyMACLayer(recMsg);
                    break;
                case PUEND:
                    LOG("SS: PU Communication ends. Setting PU channel "<<recMsg->getProposedChannel()<<" as free.");
                    channelState(recMsg->getProposedChannel(), 0);
                    notifyMACLayer(recMsg);
                    break;
                default:
                    LOG("SS: Unknown message from outside world.");
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
            LOG("Proposed channel is "<<recMsg->getChannelID());
            scheduleSensing(SenseProposedCHANNEL);
            break;
        case SenseDrmCHANNEL:
            drmChannel = recMsg->getChannelID();
            scheduleSensing(SenseDrmCHANNEL);
            break;
        default:
            LOG("SS: Unknown ctrl msg from MAC");
            break;
        }
        delete recMsg;
    }
    else if (msg->arrivedOn("mobilityInterface$i"))  // A control msg from mobility component
    {
        ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);
        switch(recMsg->getKind())
        {
            case SenseChannelMap:
                //!
                scheduleSensing(SenseChannelMap);
                break;
            default:
                LOG("SS: Unknown ctrl msg from mobility component");
                break;
        }
        delete recMsg;
    }
    else
    {
        LOG("PROBLEM in SS");
        delete msg;
    }

    // Types of sensing request. Sense particular channel, Sense until finding free channel; sensing based on DRM recommendation
    // Based upon the request of MAC or configuration
}
void MobilitySpecSensor::channelState(int chID, int chState)
{
    channelsArray[chID-1] = chState;
}
void MobilitySpecSensor::senseChannel(int type)   // NEED TO TAKE CARE OF WHEN TO CANCEL THESE TIMERS IN CASE THE NODE BECOMES RECEIVER.
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
                LOG("All Channels are busy and not turning free.");
                senseReply->setChannelState(false);
                senseReply->setKind(SenseFreeREPLY);
                senseReply->setChannelID(0);
                send(senseReply, "macInterface$o");
            }
            else  // we can continue to look for next channel to sense
            {
                emit(sensingSignal, sensedChannel+1);
                emit(sensedChannelSignal,sensedChannel+1);
                delete senseReply;
                LOG("Channel "<<sensedChannel+1<<" is busy. Sensing another");
                sensedChannel++;
                if (sensedChannel >= totalChannels)
                    sensedChannel = 0;
                scheduleSensing(SenseFreeCHANNEL);  // set another timer to sense next channel.
            }
        }
        else  // the sensed channel is free. Notify MAC Layer and give the channel ID
        {
            emit(sensingSignal, sensedChannel+1);
            emit(sensedChannelSignal,sensedChannel+1);
            LOG("Channel "<<sensedChannel+1<<" is free.");
            currentDataChannel = sensedChannel+1;
            senseReply->setChannelID(sensedChannel+1);
            senseReply->setKind(SenseFreeREPLY);
            send(senseReply, "macInterface$o");
        }
        break;
    case SenseDataCHANNEL:   // sense the state of SU channel
        if(channelsArray[currentDataChannel-1] == 0)  // data channel is still free.
        {
            emit(sensingSignal, currentDataChannel);
            emit(sensedChannelSignal,sensedChannel);
            LOG("Channel "<<currentDataChannel<<" is still free for communication");
            senseReply->setChannelState(true);
            senseReply->setKind(SenseDataREPLY);
            send(senseReply, "macInterface$o");
        }
        else // Lost data channel to PU.
        {
            sensingIteration = 0;
            emit(sensingSignal, currentDataChannel);
            emit(sensedChannelSignal,sensedChannel);
            LOG("Channel "<<currentDataChannel<<" is no longer free for communication");
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
            emit(sensedChannelSignal,sensedChannel);
            LOG("Proposed Channel "<<proposedChannel<<" is free");
            senseReply->setChannelState(true);
            senseReply->setKind(SenseProposedREPLY);
            send(senseReply, "macInterface$o");
        }
        else
        {
            LOG("Proposed channel "<<proposedChannel<<" is busy");
            emit(sensingSignal, proposedChannel);
            emit(sensedChannelSignal,sensedChannel);
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
            emit(sensedChannelSignal,drmChannel);
            LOG("DRM Channel "<<drmChannel<<" is free");
            senseReply->setChannelState(true);
        }
        else
        {
            LOG("DRM channel "<<drmChannel<<" is busy");
            emit(sensingSignal, drmChannel);
            emit(sensedChannelSignal,drmChannel);
            senseReply->setChannelState(false);
        }
        senseReply->setKind(SenseDrmREPLY);
        send(senseReply, "macInterface$o");
        break;
    case SenseChannelMap:
        senseReply->setSensingResultArraySize(totalChannels);
        for(int ch = 0; ch < totalChannels; ch++)
        {
            if ( currentDataChannel > 0 && ch == (currentDataChannel-1) )
                senseReply->setSensingResult(ch, 1);
            else
                senseReply->setSensingResult(ch, channelsArray[ch]);
        }
        senseReply->setKind(SenseChannelMapReply);
        send(senseReply, "mobilityInterface$o");
        break;
    default:
        LOG("Unknown channel sense request");
        break;
    }
}
void MobilitySpecSensor::scheduleSensing(int type)
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
    case SenseChannelMap:
        mobilitySenseTimer = new timerMsg("Mobility-Sense-Map");
        mobilitySenseTimer->setKind(SenseChannelMap);
        scheduleAt(simTime()+sensingDuration, mobilitySenseTimer);
        break;
    default:  // nothing
        break;
    }
}
void MobilitySpecSensor::init()
{
    totalChannels = getParentModule()->getParentModule()->par("totalChannels");
    drmChannel = 1;
    channelsArray = new int[totalChannels];
    sensingDuration = par("sensingDuration");
    myAddress = getParentModule()->par("address");
    broadcastAddress = getParentModule()->par("broadcastAddress");
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
void MobilitySpecSensor::publishSensingResults()
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


void MobilitySpecSensor::notifyMACLayer(dataMsg *msg)
{
    if (msg->getProposedChannel() == currentDataChannel)
    {
        currentDataChannel = 0;

        //MAC Layer
        ctrlMsg *puAppear = new ctrlMsg("MAC-Notification");
        puAppear->setKind(msg->getKind());
        send(puAppear, "macInterface$o");
    }
    delete msg;
}
