/*
 * Shah Nawaz Khan
 * shah-nawaz.khan@tu-ilmenau.de
 *
 * This application layer does not send real data. Instead it only send a request through the layers below to the MAC layer. The MAC
 * Layer creates a random amount of data packets (specified in .ini file) and attempts to send it to a destination
 */

#ifndef MOBILITYCOMPONENT_H_
#define MOBILITYCOMPONENT_H_

#include "BaseMobilityComponent.h"
#include "ctrlMsg_m.h"
#include "mobilityMsg_m.h"
#include "Logging.h"

#include <algorithm>
#include <iterator>
#include <set>

#define stringify( name ) # name

enum MachineState {
 State_INIT = 0,
 State_ONE = 1,
 State_TWO = 2,
 State_THREE = 3,
 State_FOUR = 4,
 State_FIVE = 5,
 State_SIX = 6
};

enum StateMachine_Event
{
 evSTEP= 0,
 evFREE_CHANNEL_FOUND = 1,
 evMOBILITY_SUCCESSFUL = 2,
 evMOBILITY_TIMEOUT = 3,
 evRENDEZVOUS_SUCCESS = 4,
 evBROADCAST_TIMEOUT = 5,
 evBROADCAST_BACKUP_CHANNEL_LIST_RECEIVED = 6,
 evBROADCAST_NACK_BACKUP_CHANNEL_LIST_RECEIVED = 7,
 evBROADCAST_UPDATE_BACKUP_CHANNEL_LIST_RECEIVED = 8,
 evNO_COMMON_BACKUP_CHANNEL_AVAILABLE = 9,
 evCHANNEL_SENSE_REPLY = 10
};

const char* StateMachine_StateNames[] =
{
    stringify( State_INIT ),
    stringify( State_ONE ),
    stringify( State_TWO ),
    stringify( State_THREE ),
    stringify( State_FOUR ),
    stringify( State_FIVE ),
    stringify( State_SIX )
};

const char* StateMachine_EventNames[] =
{
 stringify( evSTEP ),
 stringify( evFREE_CHANNEL_FOUND ),
 stringify( evMOBILITY_SUCCESS ),
 stringify( evMOBILITY_TIMEOUT ),
 stringify( evRENDEZVOUS_SUCCESS ),
 stringify( evBROADCAST_TIMEOUT ),
 stringify( evBROADCAST_BACKUP_CHANNEL_LIST_RECEIVED ),
 stringify( evBROADCAST_NACK_BACKUP_CHANNEL_LIST_RECEIVED ),
 stringify( evBROADCAST_UPDATE_BACKUP_CHANNEL_LIST_RECEIVED ),
 stringify( evNO_COMMON_BACKUP_CHANNEL_AVAILABLE ),
 stringify( evCHANNEL_SENSE_REPLY )
};

class MobilityComponent : public BaseMobilityComponent
{
private:
    //Stats
    simsignal_t scanChannelMapRequest,scanChannelMapReply,rendezvousSuccess,rendezvousFail;
    simsignal_t stateSignal;
    simsignal_t broadcastTimerCount;
    simsignal_t txMobilitySignal, rxMobilitySignal;
    simsignal_t stateMachineVector, eventSignal;

    int myAddress;
    int broadcastAddress;
    int masterNode;
    bool isMaster;
    bool commonBackupChannel;
    MachineState state;
    std::map<int, std::set<unsigned int> > nodeChannelsMap;
    std::map< int, std::set<unsigned int> >::iterator nodeChannelsMapIterator;
    std::set<unsigned int> channelSet;
    std::set<unsigned int>::iterator channelSetIterator;
    std::set<unsigned int> backupChannelSet;
    std::set<unsigned int> proposedBackupChannelSet;
    double broadcastTimerDuration;
    cMessage *broadcastTimer, *stateMachineStepTimer;

    void updateGUI();
    void handleSenseChannelMap(ctrlMsg *msg);
    void handleSenseChannelMapReply(cMessage *msg);
    void handleRendezvousSuccess(cMessage *msg);
    void handleRendezvousFail(cMessage *msg);
    void handleMobilityMessage(cMessage *msg);
    void senseRequest(CtrlType senseType);
    void dumpChannelMap();
    void clearBroadcastTimer();
    void setBroadcastTimer();
    void clearStateMachineStepTimer();
    void setStateMachineStepTimer();
    void broadcastBackupChannelList();
    void getCommonBackupChannel();
    void registerNeighborsChannelsFromPacket(MobilityPacket *mobPacket);
    void registerMasterProposedChannelsFromPacket(MobilityPacket *mobPacket);
    std::string printChannelSet( std::set<unsigned int> *chSet);
    void determineMaster();
    bool existsCommonBackupChannel();
    void updateBackupChannel();
    void broadcastNACKBackupChannelList();
    void broadcastCommonBackupChannel();
    void clearAllChannelSets();

    void handleState_INIT(cMessage * msg, StateMachine_Event event);
    void handleState_ONE(cMessage * msg, StateMachine_Event event);
    void handleState_TWO(cMessage * msg, StateMachine_Event event);
    void handleState_THREE(cMessage * msg, StateMachine_Event event);
    void handleState_FOUR(cMessage * msg, StateMachine_Event event);
    void handleState_FIVE(cMessage * msg, StateMachine_Event event);
    void handleState_SIX(cMessage * msg, StateMachine_Event event);

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(MobilityComponent);

#endif /* MOBILITYCOMPONENT_H_ */
