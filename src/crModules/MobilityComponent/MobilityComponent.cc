
/*
 * rainer.schuth@tu-ilmenau.de
 *
 * Based on the implementation from
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 */

#include "MobilityComponent.h"

void MobilityComponent::initialize()
{
    // Statistics
    scanChannelMapRequest = registerSignal("scanChannelMapRequest");
    scanChannelMapReply = registerSignal("scanChannelMapReply");
    rendezvousSuccess = registerSignal("rendezvousSuccess");
    rendezvousFail = registerSignal("rendezvousFail");
    broadcastTimerCount = registerSignal("broadcastTimerCount");
    stateSignal = registerSignal("stateSignal");
    txMobilitySignal = registerSignal("txMobilitySignal");
    rxMobilitySignal = registerSignal("rxMobilitySignal");
    stateMachineVector = registerSignal("stateMachineVector");
    eventSignal = registerSignal("eventSignal");

    broadcastTimerDuration = par("broadcastTimerDuration");
    myAddress = getParentModule()->par("address");
    broadcastAddress = getParentModule()->par("broadcastAddress");
    state = State_INIT;
    stateMachineStepTimer = new cMessage("StateMachineStepTimer");
    broadcastTimer = new cMessage("BroadcastTimer");
    updateGUI();
}

void MobilityComponent::updateGUI()
{
    // GUI changes
    std::stringstream sstr;
    sstr << this->getDisplayString().str()
        << ";t=" << StateMachine_StateNames[state] << "\n"
        << "MyCH:" << printChannelSet(&nodeChannelsMap[myAddress]) << "\n"
        << "BCSet:" << printChannelSet(&channelSet);
    std::string str1 = sstr.str();
    this->setDisplayString(str1.c_str());
}

void MobilityComponent::handleMessage(cMessage *msg)
{
    ASSERT(msg);
    StateMachine_Event event;

    if (msg->arrivedOn("ssInterface$i")) // ctrl msg
    {
        ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg->dup());
        switch(recMsg->getKind())
        {
            case SenseChannelMap:
                break;
            case SenseChannelMapReply:
                event = evCHANNEL_SENSE_REPLY;
                break;
            default:
                LOG("UPS ssInterface!");
                ASSERT(false);
                break;
        }
        delete recMsg;
    }
    else if(msg->arrivedOn("ctrlInterface$i")) // ctrl msg
    {
        ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg->dup());
        switch(recMsg->getKind())
        {
            case RendezvousSuccess:
                event = evRENDEZVOUS_SUCCESS;
                break;
            case RendezvousFail:
                event = evNO_COMMON_BACKUP_CHANNEL_AVAILABLE;
                break;
            default:
                LOG("UPS ctrlInterface! MsgType:" << recMsg->getKind());
                ASSERT(false);
                break;
        }
        delete recMsg;
    }
    else if(msg->arrivedOn("mobilityInterface$i"))
    {
        MobilityPacket *recMsg = check_and_cast<MobilityPacket *>(msg->dup());
        switch(recMsg->getKind())
        {
            case MOB_DATA:
                break;
            case MOB_ACK:
                break;
            case MOB_NACK:
                event = evBROADCAST_NACK_BACKUP_CHANNEL_LIST_RECEIVED;
                break;
            case MOB_EVENT:
                break;
            case MOB_PUBLISH:
                event = evBROADCAST_BACKUP_CHANNEL_LIST_RECEIVED;
                break;
            case MOB_UPDATE:
                event = evBROADCAST_UPDATE_BACKUP_CHANNEL_LIST_RECEIVED;
                break;
            case MOB_KEEPALIVE:
                break;
            case MOB_UNKNOWN:
                break;
            default:
                ASSERT(false);
        }
        delete recMsg;
        emit(rxMobilitySignal,1);
    }
    else if (msg->isSelfMessage() && msg == broadcastTimer)
    {
        emit(broadcastTimerCount,1);
        event = evBROADCAST_TIMEOUT;
    }
    else if (msg->isSelfMessage())
    {
        event = evSTEP;
        delete msg;
        msg = NULL;
    }
    else
    {
        LOG("MobilityComponent::handleMessage. Arrived on:" << msg->getArrivalGate()->getFullName());
        ASSERT(false);
    }

    emit(stateMachineVector,state);
    emit(eventSignal,event);
    updateGUI();
    LOG("I am in state:" << StateMachine_StateNames[state] << " --> " << StateMachine_EventNames[event]);
    switch (state)
    {
        case State_INIT:
            handleState_INIT(msg, event);
            break;
        case State_ONE:
            handleState_ONE(msg,event);
            break;
        case State_TWO:
            handleState_TWO(msg,event);
            break;
        case State_THREE:
            handleState_THREE(msg,event);
            break;
        case State_FOUR:
            handleState_FOUR(msg,event);
            break;
        case State_FIVE:
            handleState_FIVE(msg,event);
            break;
        case State_SIX:
            handleState_SIX(msg,event);
            break;
        default:
            ASSERT(false);
    }
    LOG("Goto state:" << StateMachine_StateNames[state]);
    updateGUI();
}

void MobilityComponent::handleSenseChannelMap(ctrlMsg *msg)
{
    ASSERT(msg);
    LOG("HandleSenseChannelMap, this should not happen!");
    delete msg;
}

void MobilityComponent::handleSenseChannelMapReply(cMessage *msg)
{
    ASSERT(msg);
    emit(scanChannelMapReply,1);

    ctrlMsg *recMsg = check_and_cast<ctrlMsg *>(msg);

    nodeChannelsMap[myAddress].clear();
    unsigned int length = recMsg->getSensingResultArraySize();
    for (unsigned int ch = 0; ch < length; ch++)
    {
        if( recMsg->getSensingResult(ch) == 0 )
            nodeChannelsMap[myAddress].insert(ch+1); //Insert the free-channel id
    }

    dumpChannelMap();
    delete recMsg;
}

void MobilityComponent::handleRendezvousSuccess(cMessage *msg)
{
    ASSERT(msg);
    emit(rendezvousSuccess,1);
    senseRequest(SenseChannelMap);
    delete msg;
}

void MobilityComponent::handleRendezvousFail(cMessage *msg)
{
    ASSERT(msg);
    emit(rendezvousFail,1);
    delete msg;
}

void MobilityComponent::senseRequest(CtrlType senseType)
{

    ctrlMsg *senseRequest = new ctrlMsg("SenseRequest");
    switch (senseType)
    {
        case SenseFreeCHANNEL:  // sense any free channel request
            senseRequest->setKind(SenseFreeCHANNEL);
            break;
        case SenseDataCHANNEL:  // sense the data channel request
            break;
        case SenseProposedCHANNEL:  // sense the channel proposed by Tx
            break;
        case SenseChannelMap:
            emit(scanChannelMapRequest,1);
            senseRequest->setKind(SenseChannelMap);;
            break;
        default:
            LOG("Unknown sensing request");
            delete senseRequest;
            return;
            break;
    }
    send(senseRequest, "ssInterface$o");
}

void MobilityComponent::dumpChannelMap()
{
    //iterate over the channel map
    for( nodeChannelsMapIterator = nodeChannelsMap.begin();
        nodeChannelsMapIterator != nodeChannelsMap.end();
        nodeChannelsMapIterator++ )
    {
        int addr = nodeChannelsMapIterator->first;
        std::set<unsigned int> chSet = nodeChannelsMapIterator->second;
        std::ostringstream st;
        //iterate over the channelSet of a node
        for (channelSetIterator = chSet.begin();
            channelSetIterator != chSet.end();
            ++channelSetIterator)
        {
            st << (*channelSetIterator) << "|";
        }
        LOG("DumpChannelMap:: Addr: " << addr << " | channel set size: " <<  chSet.size() << " channel set: " << st.str());
    }
}

void MobilityComponent::clearAllChannelSets()
{
    //iterate over the channel map
    for( nodeChannelsMapIterator = nodeChannelsMap.begin();
        nodeChannelsMapIterator != nodeChannelsMap.end();
        nodeChannelsMapIterator++ )
    {
        nodeChannelsMapIterator->second.clear();
    }
}

void MobilityComponent::clearBroadcastTimer()
{
    if ( broadcastTimer != NULL )
    {
        LOG("Broadcast timer cleared");
        cancelAndDelete(broadcastTimer);
        broadcastTimer = NULL;
    }
}

void MobilityComponent::setBroadcastTimer()
{
    if ( broadcastTimer == NULL )
    {
        LOG("Broadcast timer set");
        broadcastTimer = new cMessage("BroadcastTimer");
        scheduleAt(simTime()+broadcastTimerDuration, broadcastTimer);
    }
}

void MobilityComponent::clearStateMachineStepTimer()
{
    if ( stateMachineStepTimer != NULL )
    {
        LOG("stateMachineStepTimer cleared");
        cancelAndDelete(stateMachineStepTimer);
        stateMachineStepTimer = NULL;
    }
}

void MobilityComponent::setStateMachineStepTimer()
{
    //if ( stateMachineStepTimer == NULL )
    //{
        //LOG("stateMachineStepTimer set");
        //stateMachineStepTimer = new cMessage("StateMachineStepTimer");
        //scheduleAt(simTime()+0.00001, stateMachineStepTimer);
    //}
    cMessage * stepTimer = new cMessage("StateMachineStepTimer");
    scheduleAt(simTime()+0.00001, stepTimer);
}

void MobilityComponent::broadcastBackupChannelList()
{
    LOG("------------------------------------broadcastBackupChannelList()");
    //generatePUBLISHPacket
    MobilityPacket * mobPacket = new MobilityPacket("MobPUBLISHPacket");
    mobPacket->setKind(MOB_PUBLISH);

    //addChannelsToMobilityPacket
    std::set<unsigned int> chSet = nodeChannelsMap[myAddress];

    mobPacket->setChannelMapArraySize(chSet.size());

    //iterate over the channelSet
    int i = 0;
    for (channelSetIterator = chSet.begin();
        channelSetIterator != chSet.end();
        channelSetIterator++)
    {
        mobPacket->setChannelMap(i,(*channelSetIterator));
        i++;
    }

    //Generate broadcast
    mobPacket->setSource(myAddress);
    mobPacket->setDestination(broadcastAddress);

    //sendMobilityPacket
    send(mobPacket, "mobilityInterface$o");
    clearBroadcastTimer();
    setBroadcastTimer();
    emit(txMobilitySignal,1);
}

void MobilityComponent::getCommonBackupChannel()
{
    channelSet.clear();
    channelSet.insert( nodeChannelsMap[myAddress].begin(), nodeChannelsMap[myAddress].end() );

    ////iterate over the channel map
    for( nodeChannelsMapIterator = nodeChannelsMap.begin();
        nodeChannelsMapIterator != nodeChannelsMap.end();
        ++nodeChannelsMapIterator )
    {

      ////iterate over the others node map
      if ( nodeChannelsMapIterator->first != myAddress )
      {
        std::set<unsigned int> Result;
        std::set_intersection( channelSet.begin(), channelSet.end(),
            nodeChannelsMapIterator->second.begin(), nodeChannelsMapIterator->second.end(),
                std::inserter( Result, Result.begin() ) );

        channelSet.clear();
        channelSet.insert( Result.begin(), Result.end() );

      }
    }

    LOG( "getCommonBackupChannel() " << printChannelSet(&channelSet) );
}

void MobilityComponent::determineMaster()
{
    std::set < int > nodeSet;

    //iterate over the channel and node map
    for( nodeChannelsMapIterator = nodeChannelsMap.begin();
        nodeChannelsMapIterator != nodeChannelsMap.end();
        ++nodeChannelsMapIterator)
    {
      // get each node and determine the master node
      nodeSet.insert( nodeChannelsMapIterator->first);
    }

    masterNode = *nodeSet.begin();
    if ( masterNode == myAddress )
    {
        isMaster = true;
        LOG("I am the MASTER node!");
    }
    else
    {
        isMaster = false;
        LOG("I am a SLAVE Node!");
    }

}

std::string MobilityComponent::printChannelSet( std::set<unsigned int> *chSet)
{
    ASSERT(chSet);
    std::ostringstream st;
    //st << "Print channel set:: | channel set size: " <<  chSet->size() << " channel set: ";
    //iterate over the channelSet of a node
    for (channelSetIterator = chSet->begin();
        channelSetIterator != chSet->end();
        ++channelSetIterator)
    {
        st << (*channelSetIterator) << "|";
    }

    return st.str();
}

void MobilityComponent::registerNeighborsChannelsFromPacket(MobilityPacket *mobPacket)
{
    ASSERT(mobPacket);

    int source = mobPacket->getSource();

    nodeChannelsMap[source].clear();

    int mapSize = mobPacket->getChannelMapArraySize();
    for( int i=0; i < mapSize; i++)
    {
        int ch = mobPacket->getChannelMap(i);
        nodeChannelsMap[source].insert(ch);
    }

    if ( masterNode != -1 && source == masterNode )
    {
        proposedBackupChannelSet.clear();
        proposedBackupChannelSet.insert( nodeChannelsMap[source].begin(), nodeChannelsMap[source].end() );
    }

    dumpChannelMap();
}

void MobilityComponent::registerMasterProposedChannelsFromPacket(MobilityPacket *mobPacket)
{
    ASSERT(mobPacket);
    //int source = mobPacket->getSource();
    //if ( masterNode != -1 && source == masterNode )
    //{
        //proposedBackupChannelSet.clear();
        //nodeChannelsMap[source].clear();

        //int mapSize = mobPacket->getChannelMapArraySize();
        //for( int i=0; i < mapSize; i++)
        //{
            //int ch = mobPacket->getChannelMap(i);
            //proposedBackupChannelSet.insert(ch);
            //nodeChannelsMap[source].insert(ch);
        //}
    //}
    //dumpChannelMap();
}

bool MobilityComponent::existsCommonBackupChannel()
{
    //Check if master node BackupChannelSet and mine are equal
    LOG("proposedBackupChannelSet: " << printChannelSet(&proposedBackupChannelSet));
    LOG("nodeChannelsMap[myAddress]: " << printChannelSet(&nodeChannelsMap[myAddress]));
    LOG("channelSet :" << printChannelSet(&channelSet));
    if ( proposedBackupChannelSet.empty() )
    {
        LOG(1);
        commonBackupChannel = !channelSet.empty();
    }
    else if ( ! proposedBackupChannelSet.empty() &&
            std::equal(proposedBackupChannelSet.begin(), proposedBackupChannelSet.end(),
            channelSet.begin()) )
    {
        LOG(2);
        commonBackupChannel = true;
    }
    else
    {
        LOG(3);
        commonBackupChannel = false;
    }
    LOG("**** CommonBackupChannel? " << commonBackupChannel);
    return commonBackupChannel;
}

void MobilityComponent::updateBackupChannel()
{
    backupChannelSet.clear();

    if( ! channelSet.empty() )
    {
        backupChannelSet.insert( channelSet.begin(), channelSet.end() );
        LOG( "Update common backup channel to: " << printChannelSet(&backupChannelSet) );
        //activateEvent("evmobilitysuccessful", n );
    }
    else
    {
      LOG( "No common backup channel! Clear set.");
      //activateEvent("evmobilitysuccessful", n);
    }
}

void MobilityComponent::broadcastNACKBackupChannelList()
{
    //!FIXME: create mob packet and type function, set type function

    LOG("------------------------------------broadcastNACKBackupChannelList()");
    //generateNACKPacket
    MobilityPacket * mobPacket = new MobilityPacket("MobNACKPacket");
    mobPacket->setKind(MOB_NACK);

    //addChannelsToMobilityPacket
    std::set<unsigned int> chSet = nodeChannelsMap[myAddress];

    mobPacket->setChannelMapArraySize(chSet.size());

    //iterate over the channelSet
    int i = 0;
    for (channelSetIterator = chSet.begin();
        channelSetIterator != chSet.end();
        channelSetIterator++)
    {
        mobPacket->setChannelMap(i,(*channelSetIterator));
        i++;
    }

    //Generate broadcast
    mobPacket->setSource(myAddress);
    mobPacket->setDestination(broadcastAddress);

    //sendMobilityPacket
    send(mobPacket, "mobilityInterface$o");
    clearBroadcastTimer();
    emit(txMobilitySignal,1);
}

void MobilityComponent::broadcastCommonBackupChannel()
{
    //!FIXME: create mob packet and type function, set type function

    LOG("------------------------------------broadcastCommonChannelList()");
    //generateNACKPacket
    MobilityPacket * mobPacket = new MobilityPacket("MobUPDATEPacket");
    mobPacket->setKind(MOB_UPDATE);

    //addChannelsToMobilityPacket
    mobPacket->setChannelMapArraySize(channelSet.size());

    //iterate over the channelSet
    int i = 0;
    for (channelSetIterator = channelSet.begin();
        channelSetIterator != channelSet.end();
        channelSetIterator++)
    {
        mobPacket->setChannelMap(i,(*channelSetIterator));
        i++;
    }

    //Generate broadcast
    mobPacket->setSource(myAddress);
    mobPacket->setDestination(broadcastAddress);

    //sendMobilityPacket
    send(mobPacket, "mobilityInterface$o");
    clearBroadcastTimer();
    setBroadcastTimer();
    emit(txMobilitySignal,1);
}

void MobilityComponent::handleMobilityMessage(cMessage *msg)
{
    ASSERT(msg);
    MobilityPacket *mobPacket = check_and_cast<MobilityPacket *>(msg->dup());
    switch( mobPacket->getKind() )
    {
        case MOB_NACK:
        case MOB_PUBLISH:
        case MOB_UPDATE:
            registerNeighborsChannelsFromPacket(mobPacket);
            getCommonBackupChannel();
            break;
        default:
            LOG("UPS! " << mobPacket->getKind());
            ASSERT(false);
    }
    delete mobPacket;
}

void MobilityComponent::handleState_INIT(cMessage * msg, StateMachine_Event event)
{
    //Clear channel status
    //delete old scanned channels status data
    clearAllChannelSets();

    isMaster = false;
    masterNode = -1;
    nodeChannelsMap.clear();

    //Check initialization conditions
    switch ( event )
    {
        case evRENDEZVOUS_SUCCESS:
        {
            handleRendezvousSuccess(msg);
            state = State_INIT;
            break;
        }
         case evCHANNEL_SENSE_REPLY:
        {
            handleSenseChannelMapReply(msg);
            broadcastBackupChannelList();
            state = State_ONE;
            break;
        }
        default:
            break;
    }
}

void MobilityComponent::handleState_ONE(cMessage * msg, StateMachine_Event event)
{
    switch ( event )
    {
        case evNO_COMMON_BACKUP_CHANNEL_AVAILABLE:
        {
            clearBroadcastTimer();
            handleRendezvousFail(msg);
            state = State_INIT;
            setStateMachineStepTimer();
            break;
        }
        case evBROADCAST_TIMEOUT:
        {
            broadcastBackupChannelList();
            state = State_TWO;
            break;
        }
        case evBROADCAST_UPDATE_BACKUP_CHANNEL_LIST_RECEIVED:
        case evBROADCAST_NACK_BACKUP_CHANNEL_LIST_RECEIVED:
        case evBROADCAST_BACKUP_CHANNEL_LIST_RECEIVED:
        {
            clearBroadcastTimer();
            //registerNeighborsChannelsFromPacket(msg);
            //getCommonBackupChannel();
            handleMobilityMessage(msg);
            state = State_THREE;
            //clearStateMachineStepTimer();
            setStateMachineStepTimer();
            break;
        }
        default:
            break;
    }
    //setStateMachineStepTimer();
}

void MobilityComponent::handleState_TWO(cMessage * msg, StateMachine_Event event)
{
    switch ( event )
    {
        case evNO_COMMON_BACKUP_CHANNEL_AVAILABLE:
        {
            clearBroadcastTimer();
            handleRendezvousFail(msg);
            state = State_INIT;
            setStateMachineStepTimer();
            break;
        }
        case evBROADCAST_TIMEOUT:
        {
            getCommonBackupChannel();
            state = State_THREE;
            //clearStateMachineStepTimer();
            setStateMachineStepTimer();
            break;
        }
        default:
            break;
    }
    //setStateMachineStepTimer();
}

void MobilityComponent::handleState_THREE(cMessage * msg, StateMachine_Event event)
{
    switch ( event )
    {
        case evNO_COMMON_BACKUP_CHANNEL_AVAILABLE:
        {
            clearBroadcastTimer();
            handleRendezvousFail(msg);
            state = State_INIT;
            setStateMachineStepTimer();
            break;
        }
        case evSTEP:
        {
            determineMaster();
            state = State_FOUR;
            //clearStateMachineStepTimer();
            setStateMachineStepTimer();
            break;
        }
        default:
            break;
    }
    //clearBroadcastTimer();
    //setStateMachineStepTimer();
}

void MobilityComponent::handleState_FOUR(cMessage * msg, StateMachine_Event event)
{
    switch ( event )
    {
        case evNO_COMMON_BACKUP_CHANNEL_AVAILABLE:
        {
            clearBroadcastTimer();
            handleRendezvousFail(msg);
            state = State_INIT;
            setStateMachineStepTimer();
            break;
        }
        case evBROADCAST_TIMEOUT:
        {
            if( ! isMaster && existsCommonBackupChannel() )
            {
                updateBackupChannel();
            }
            else if( ! isMaster && ! existsCommonBackupChannel() )
            {
                broadcastNACKBackupChannelList();
            }
            state = State_FIVE;
            //clearStateMachineStepTimer();
            setStateMachineStepTimer();
            break;
        }
        case evBROADCAST_UPDATE_BACKUP_CHANNEL_LIST_RECEIVED:
        case evBROADCAST_NACK_BACKUP_CHANNEL_LIST_RECEIVED:
        case evBROADCAST_BACKUP_CHANNEL_LIST_RECEIVED:
        {
            //clearBroadcastTimer();
            //registerNeighborsChannelsFromPacket(msg);
            //getCommonBackupChannel();
            handleMobilityMessage(msg);
            state = State_THREE;
            setStateMachineStepTimer();
            break;
        }

        case evSTEP:
        {
            if( isMaster )
            {
                if( existsCommonBackupChannel() )
                {
                    broadcastCommonBackupChannel();
                    clearBroadcastTimer(); //Cancel the timer, we dont need the timer
                }
                state = State_SIX;
                //clearStateMachineStepTimer();
                setStateMachineStepTimer();
            }
            else
            {
                state = State_FOUR;
                setBroadcastTimer();
            }
            break;
        }
        default:
            LOG("***** This should not happen?");
            setStateMachineStepTimer();
            break;
    }
}

void MobilityComponent::handleState_FIVE(cMessage * msg, StateMachine_Event event)
{
    switch ( event )
    {
        case evNO_COMMON_BACKUP_CHANNEL_AVAILABLE:
        {
            clearBroadcastTimer();
            handleRendezvousFail(msg);
            state = State_INIT;
            setStateMachineStepTimer();
            break;
        }
        case evRENDEZVOUS_SUCCESS:
        {
            if ( isMaster )
            {
                handleRendezvousSuccess(msg);
                state = State_FIVE;
            }
            break;
        }
        case evBROADCAST_UPDATE_BACKUP_CHANNEL_LIST_RECEIVED:
        case evBROADCAST_NACK_BACKUP_CHANNEL_LIST_RECEIVED:
        case evBROADCAST_BACKUP_CHANNEL_LIST_RECEIVED:
        {
            clearBroadcastTimer();
            //registerNeighborsChannelsFromPacket(msg);
            //getCommonBackupChannel();
            handleMobilityMessage(msg);
            state = State_THREE;
            //clearStateMachineStepTimer();
            setStateMachineStepTimer();
            break;
        }
        case evFREE_CHANNEL_FOUND:
        {
            LOG("FIXME! State_FIVE: evFREE_CHANNEL_FOUND: not implemented!");
            break;
        }
        default:
            break;
    }
}

void MobilityComponent::handleState_SIX(cMessage * msg, StateMachine_Event event)
{
    switch ( event )
    {
        case evNO_COMMON_BACKUP_CHANNEL_AVAILABLE:
        {
            clearBroadcastTimer();
            handleRendezvousFail(msg);
            state = State_INIT;
            setStateMachineStepTimer();
            break;
        }
        case evSTEP:
        {
            if ( isMaster )
            {
                updateBackupChannel();
                state = State_FIVE;
            }
            else
            {
                state = State_FOUR;
            }
            //clearStateMachineStepTimer();
            setStateMachineStepTimer();
            break;
        }
        default:
            break;
    }
    //clearStateMachineStepTimer();
    //setStateMachineStepTimer();
}
