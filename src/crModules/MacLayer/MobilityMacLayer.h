
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

#ifndef MOBILITYMACLAYER_H_
#define MOBILITYMACLAYER_H_

#include "BaseMacLayer.h"
#include "ctrlMsg_m.h"
#include "dataMsg_m.h"
#include "timerMsg_m.h"
#include "mobilityMsg_m.h"

#define stringify( name ) # name

enum MacLayerState {
    IDLE = 0,
    RTS_SENDING = 1,
    //RTS_RECEIVING,
    RTS_TIMEOUT = 2,
    RTS_SENT = 3,
    RTS_RECEIVED = 4,
    //CTS_SENDING,
    //CTS_RECEIVING,
    CTS_TIMEOUT = 5,
    CTS_SENT = 6,
    CTS_RECEIVED = 7,
    RENDEZVOUS_ACHIEVED = 8,
    DATA_SENDING = 9,
    DATA_RECEIVING = 10,
    DATA_TIMEOUT = 11,
    DATA_SENT = 12,
    //DATA_RECEIVED,
    SCANNING_SPECTRUM = 13,
    TIMEOUT_SPECTRU = 14,
    HANDOVER = 15
};

const char* MacLayerState_Names[] =
{
    stringify( IDLE ),
    stringify( RTS_SENDING ),
    //RTS_RECEIVING,
    stringify( RTS_TIMEOUT ),
    stringify( RTS_SENT ),
    stringify( RTS_RECEIVED ),
    //CTS_SENDING,
    //CTS_RECEIVING,
    stringify( CTS_TIMEOUT ),
    stringify( CTS_SENT ),
    stringify( CTS_RECEIVED ),
    stringify( RENDEZVOUS_ACHIEVED ),
    stringify( DATA_SENDING ),
    stringify( DATA_RECEIVING ),
    stringify( DATA_TIMEOUT ),
    stringify( DATA_SENT ),
    //DATA_RECEIVED,
    stringify( SCANNING_SPECTRUM ),
    stringify( TIMEOUT_SPECTRU ),
    stringify( HANDOVER )
};


class MobilityMacLayer : public BaseMacLayer {

private:

    int numberOfPackets, totalFrames;  // The number of packets to send in response to Application request.
    bool perPacketSensing;   // Should the transmission channel be sensed per packet or per session
    bool drmEnabled;
    bool ackEnabled;
    std::map<int, std::string> colorMap;
    int rxSeqNo, txSeqNo;
    MacLayerState currentState;
    int myAddress;  // MAC address of node
    int broadcastAddress;
    int currentDestination;  // Keeps track on next hop or destination
    int currentDataChannel;    // The channel currently used for data transmission.
    int proposedChannel;
    int drmChannel;
    bool rtsSent;         // to keep track of sent RTS
    timerMsg *rtsTimer, *ackTimer;   // RTS timer for next attempt and ACK timeout
    bool isReceiving, isTransmitting;   // keeps track of radio state
    int rtsAttempts, ctsDestination; // keeps track of number of retransmissions.
    void setCurrentState(MacLayerState state);
    void setCurrentDataChannel(int chID);
    void updateGUI(MacLayerState state, int chID);
    cPacketQueue *txDataQueue;
    cPacketQueue *rxDataQueue;
    cPacketQueue *txMobilityQueue;
    cPacketQueue *rxMobilityQueue;

    // Statistics
    simsignal_t rtsSignal;
    simsignal_t totalTxPacketsSignal, totalRxPacketsSignal;
    simsignal_t txDataSignal, txAckSignal;
    simsignal_t rxDataSignal, rxAckSignal;
    simsignal_t txMobilitySignal, rxMobilitySignal;
    simsignal_t handover;
    simsignal_t nackSignal;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    // Msg sending functions
    void sendRTS(int);
    void sendCTS();
    void sendData(int kind);
    void sendACK(int source, int seqNo);
    void sendNack(int);
    void sendEOT();     // Indicates the end of transmission or contention
    void sendRendezvousCONFIRMATION();
    void sendRendezvousFAIL();
    void sendAppCONFIRMATION();

    // Msg handling functions
    void handleRTS(dataMsg *msg);
    void handleCTS(dataMsg *msg);
    void handleAck(dataMsg *msg);
    void handleNack(int);
    void handleEOT();
    void handlePU();
    void handleAppREQUEST(ctrlMsg *msg);
    void handleSenseFreeREPLY(ctrlMsg *msg);
    void handleSenseDataREPLY(ctrlMsg *msg);
    void hanldeSenseProposedREPLY(ctrlMsg *msg);
    void handleSenseDrmREPLY(ctrlMsg *msg);
    void handleDataFromAbove(dataMsg *msg);
    void handleDataFromBelow(dataMsg *msg);
    void handleMobilityFromAbove(MobilityPacket *msg);
    void handleMobilityFromBelow(dataMsg *msg);
    void processData();

    // Timer Setting/Clearing functions
    void setRTStimer();
    void clearRTStimer();
    void clearAckTimer();
    void setAckTimeOut();

    // Objective functions
    void senseRequest(int);
    void getIdle();
    void notifyAppLayer(int);    // 0 as failure 1 as success
};

Define_Module(MobilityMacLayer);

#endif /* MOBILITYMACLAYER_H_ */
